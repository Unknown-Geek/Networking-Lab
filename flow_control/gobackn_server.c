/*
 * Go-Back-N Server (receiver)
 * ----------------------------
 * socket(SOCK_DGRAM) → bind()
 *   └─ loop:
 *       ├─ recvfrom(packet)
 *       ├─ check recd[] array for duplicates
 *       ├─ sleep(1)
 *       └─ rand()%3==0 → sendto(ACK)   ← randomly drop ACKs
 *                   else → drop  (client resends entire window)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
        char* ip = "127.0.0.100";
        int port = 6665;
        srand(time(0));

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

        int recd[10];
        for (int i = 0; i < 10; i++)
                recd[i] = -1;

        while (1) {
                bzero(buffer, 1024);
                addr_size = sizeof(client_addr);
                recvfrom(server_sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addr_size);

                int b = atoi(buffer);
                if (recd[b] != b) {
                        recd[b] = b;
                        printf("Server: Received packet %s\n", buffer);
                } else {
                        printf("Server: Received duplicate packet %s\n", buffer);
                }

                sleep(1);

                if (rand() % 3 == 0) {
                        printf("Server: Sending ACK for packet %s\n", buffer);
                        sendto(server_sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, addr_size);
                }
        }
        close(server_sock);
}
