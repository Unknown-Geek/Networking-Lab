/*
 * Stop-and-Wait ARQ Client (with timeout/retransmit)
 * ----------------------------------------------------
 * socket(SOCK_DGRAM)
 *   └─ loop (counter 1..3):
 *       ├─ sendto(packet)
 *       ├─ setsockopt(SO_RCVTIMEO, 5s)  ← set receive timeout
 *       ├─ recvfrom()
 *       ├─ if timeout (n<0)  → retry (counter stays same)
 *       └─ if ACK received   → counter++
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define TIMEOUT 5

int main() {
        char* ip = "127.0.0.100";
        int port = 6665;

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

        int counter = 1;

        while (counter <= 3) {
                addr_size = sizeof(server_addr);
                bzero(buffer, 1024);
                sprintf(buffer, "%d", counter);
                printf("Client: Sending packet %s\n", buffer);
                sendto(client_sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, addr_size);

                struct timeval timeout;
                timeout.tv_sec = TIMEOUT;
                timeout.tv_usec = 0;
                setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

                bzero(buffer, 1024);
                int n = recvfrom(client_sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, &addr_size);
                if (n < 0) {
                        printf("Client: Timeout! Resending packet\n");
                } else {
                        printf("Client: Received ACK for packet %s\n", buffer);
                        counter++;
                }
                sleep(1);
        }
        close(client_sock);
}
