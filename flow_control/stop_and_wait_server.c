#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
        char* ip = "127.0.0.100";
        int port = 6666;
        int packet_count = 0;

        int server_sock;
        struct sockaddr_in server_addr, client_addr;
        char buffer[1024];
        socklen_t addr_size;

        server_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (server_sock < 0) {
                perror("Socket error");
                exit(1);
        }

        memset(&server_addr, '\0', sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = inet_addr(ip);

        if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                perror("Bind error");
                exit(1);
        }

        printf("Server listening on port %d\n", port);

        while (packet_count <= 10) {
                bzero(buffer, 1024);
                addr_size = sizeof(client_addr);
                recvfrom(server_sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addr_size);
                while (1) {
                        sleep(1);
                        if (atoi(buffer) == packet_count) {
                                printf("Server: Received packet %d\n", packet_count);
                                break;
                        }
                }

                printf("Server: Sending ACK for packet %d\n", packet_count);
                sendto(server_sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, addr_size);

                bzero(buffer, 1024);
                recvfrom(server_sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addr_size);
                while (1) {
                        sleep(1);
                        if (atoi(buffer) == packet_count) {
                                printf("Server: Received ACK for packet %d\n", packet_count);
                                packet_count++;
                                break;
                        }
                }
        }
        close(server_sock);
}
