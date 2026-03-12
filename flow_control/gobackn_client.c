/*
 * Go-Back-N Client (sender)
 * --------------------------
 * socket(SOCK_DGRAM)
 *   └─ sendWindowPackets(windowStart, windowEnd)  ← send entire window at once
 *   └─ loop until all packets ACKed:
 *       ├─ setsockopt(SO_RCVTIMEO) → recvfrom()
 *       ├─ timeout / wrong ACK → resend entire window (go back N)
 *       └─ correct ACK → windowStart++ , windowEnd++, send new packet
 *
 * sendWindowPackets(): loop from windowStart to windowEnd → sendto each
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

void send_window(int client_sock, struct sockaddr_in server_addr, int packets[], int start, int end) {
        while (start < end) {
                send_packet(client_sock, server_addr, packets[start]);
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
        for (int i = 0; i < PACKET_COUNT; i++)
                packets[i] = i;

        int window_start = 0;
        int window_end = WINDOW_SIZE;

        send_window(client_sock, server_addr, packets, window_start, window_end);

        while (window_start < PACKET_COUNT) {
                bzero(buffer, 1024);
                addr_size = sizeof(server_addr);

                int n = recvfrom(client_sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, &addr_size);
                if (n < 0) {
                        printf("Client: Timeout! Resending window\n");
                        send_window(client_sock, server_addr, packets, window_start, window_end);
                } else {
                        int ack_id = atoi(buffer);
                        printf("Client: Received ACK for packet %d\n", ack_id);
                        if (ack_id != packets[window_start]) {
                                printf("Client: Wrong ACK! Resending window\n");
                                send_window(client_sock, server_addr, packets, window_start, window_end);
                        } else {
                                window_start++;
                                if (window_end < PACKET_COUNT) {
                                        send_packet(client_sock, server_addr, packets[window_end]);
                                        window_end++;
                                }
                        }
                }
                sleep(1);
        }
        close(client_sock);
}
