/*
 * Selective Repeat Client (sender)
 * ---------------------------------
 * socket(SOCK_DGRAM)
 *   └─ sendWindowPackets(windowStart, windowEnd)  ← send packets in window once
 *   └─ loop until all packets ACKed:
 *       ├─ setsockopt(SO_RCVTIMEO) → recvfrom()
 *       ├─ timeout → resend only unacked packets in current window
 *       ├─ NACK packet i → resend only packet i
 *       └─ ACK packet i → mark acked, slide window, send new packet entering window
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define TIMEOUT 12
#define PACKET_COUNT 5
#define WINDOW_SIZE 2

void send_packet(int client_sock, struct sockaddr_in server_addr, int packet) {
        char buffer[1024];
        bzero(buffer, 1024);
        sprintf(buffer, "%d", packet);
        printf("Client: Sending packet %s\n", buffer);
        sendto(client_sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
}

void send_window(int client_sock, struct sockaddr_in server_addr, int packets[], int acked[], int start, int end) {
        while (start < end) {
                if (acked[start] == 0) {
                        send_packet(client_sock, server_addr, packets[start]);
                }
                start++;
        }
}

int main() {
        char* ip = "127.0.0.100";
        int port = 6665;

        int client_sock;
        struct sockaddr_in server_addr;
        char buffer[1024];
        socklen_t addr_size;
        struct timeval timeout;

        client_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (client_sock < 0) {
                perror("Socket error");
                exit(1);
        }

        memset(&server_addr, '\0', sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = inet_addr(ip);

        timeout.tv_sec = TIMEOUT;
        timeout.tv_usec = 0;
        setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        int packets[PACKET_COUNT];
        int acked[PACKET_COUNT];
        for (int i = 0; i < PACKET_COUNT; i++) {
                packets[i] = i;
                acked[i] = 0;
        }

        int window_start = 0;
        int window_end = WINDOW_SIZE;

        send_window(client_sock, server_addr, packets, acked, window_start, window_end);

        while (window_start < PACKET_COUNT) {
                bzero(buffer, 1024);
                addr_size = sizeof(server_addr);

                int n = recvfrom(client_sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, &addr_size);
                if (n < 0) {
                        printf("Client: Timeout! Resending unacked packets in window\n");
                        send_window(client_sock, server_addr, packets, acked, window_start, window_end);
                } else {
                        char msg[20];
                        int packet_id;
                        sscanf(buffer, "%s %d", msg, &packet_id);

                        if (packet_id < 0 || packet_id >= PACKET_COUNT)
                                continue;

                        if (strcmp(msg, "NACK") == 0) {
                                printf("Client: Received NACK for packet %d\n", packet_id);
                                acked[packet_id] = 0;
                                send_packet(client_sock, server_addr, packet_id);
                        } else if (strcmp(msg, "ACK") == 0) {
                                printf("Client: Received ACK for packet %d\n", packet_id);
                                acked[packet_id] = 1;

                                while (window_start < PACKET_COUNT && acked[window_start] == 1) {
                                        window_start++;
                                        if (window_end < PACKET_COUNT) {
                                                send_packet(client_sock, server_addr, packets[window_end]);
                                                window_end++;
                                        }
                                }
                        }
                }
                sleep(1);
        }

        close(client_sock);
}
