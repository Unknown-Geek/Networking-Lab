/*
 * Stop-and-Wait Client (sender)
 * ------------------------------
 * socket(SOCK_DGRAM)   ← no bind on client side
 *   └─ loop (packet_count 0..10):
 *       ├─ sendto(packet N)
 *       ├─ recvfrom()          ← wait for ACK from server
 *       ├─ sendto(ACK N)       ← final ACK back
 *       └─ packet_count++
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
        char* ip = "127.0.0.100";
        int port = 6666;
        int packet_count = 0;

        int client_sock;
        struct sockaddr_in server_addr;
        char buffer[1024];
        socklen_t addr_size;

        client_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (client_sock < 0) {
                perror("Socket error");
                exit(1);
        }

        memset(&server_addr, '\0', sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = inet_addr(ip);

        while (packet_count <= 10) {
                addr_size = sizeof(server_addr);
                bzero(buffer, 1024);
                sprintf(buffer, "%d", packet_count);
                printf("Client: Sending packet %d\n", packet_count);
                sendto(client_sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, addr_size);

                bzero(buffer, 1024);
                recvfrom(client_sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, &addr_size);
                while (1) {
                        sleep(1);
                        if (atoi(buffer) == packet_count) {
                                printf("Client: Received ACK for packet %d\n", packet_count);
                                break;
                        }
                }

                printf("Client: Sending ACK for packet %d\n", packet_count);
                sendto(client_sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, addr_size);

                packet_count++;
        }
        close(client_sock);
}