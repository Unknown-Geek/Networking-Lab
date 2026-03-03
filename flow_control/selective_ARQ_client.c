#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define TIMEOUT 5
#define PACKET_COUNT 10
#define WINDOW_SIZE 3

typedef struct {
        int id;
        int sent;
} Packet;

Packet packets[PACKET_COUNT];
pthread_t thread[PACKET_COUNT], receive_thread;

int client_sock;
struct sockaddr_in server_addr;

int window_start = 0;
int window_end = 0;
int next_to_send = 0;

void* send_packet(void* arg) {
        Packet* packet = (Packet*)arg;
        char buffer[1024];

        while (packet->sent == 0) {
                printf("Client: Sending packet %d\n", packet->id);
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "%d", packet->id);
                sendto(client_sock, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
                sleep(TIMEOUT);
                if (packet->sent == 0)
                        printf("Client: Timeout for packet %d\n", packet->id);
        }
        return NULL;
}

void* receive_packets(void* arg) {
        socklen_t addr_size = sizeof(server_addr);
        char buffer[1024];

        while (1) {
                memset(buffer, 0, sizeof(buffer));
                int n = recvfrom(client_sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&server_addr, &addr_size);
                if (n <= 0) continue;
                buffer[n] = '\0';

                char msg[20];
                int packet_id;
                sscanf(buffer, "%s %d", msg, &packet_id);

                if (packet_id < 1 || packet_id > PACKET_COUNT) continue;

                if (strcmp(msg, "NACK") == 0) {
                        printf("Client: Received NACK for packet %d. Resending\n", packet_id);
                        packets[packet_id - 1].sent = 0;
                } else if (strcmp(msg, "ACK") == 0) {
                        printf("Client: Received ACK for packet %d\n", packet_id);
                        packets[packet_id - 1].sent = 1;

                        if (window_start == packet_id - 1) {
                                while (window_start < PACKET_COUNT && packets[window_start].sent == 1) {
                                        window_start++;
                                        if (window_end < PACKET_COUNT - 1)
                                                window_end++;
                                }
                        }

                        if (window_start >= PACKET_COUNT) break;
                }
        }
        return NULL;
}

int main() {
        for (int i = 0; i < PACKET_COUNT; i++) {
                packets[i].id = i + 1;
                packets[i].sent = 0;
        }

        char* ip = "127.0.0.100";
        int port = 6665;

        client_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (client_sock < 0) {
                perror("Socket error");
                exit(1);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = inet_addr(ip);

        pthread_create(&receive_thread, NULL, receive_packets, NULL);

        window_start = 0;
        window_end = WINDOW_SIZE - 1;
        next_to_send = 0;

        for (int i = window_start; i <= window_end && i < PACKET_COUNT; i++) {
                pthread_create(&thread[i], NULL, send_packet, &packets[i]);
                next_to_send++;
        }

        while (window_start < PACKET_COUNT) {
                if (next_to_send < PACKET_COUNT && next_to_send <= window_end) {
                        pthread_create(&thread[next_to_send], NULL, send_packet, &packets[next_to_send]);
                        next_to_send++;
                }
                sleep(1);
        }

        printf("All packets acknowledged\n");
        close(client_sock);
        return 0;
}
