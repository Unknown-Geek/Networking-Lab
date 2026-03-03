/*
 * UDP Time Server
 * ---------------
 * socket(SOCK_DGRAM) → bind()
 *   └─ loop: recvfrom(buffer)   ← wait for any request
 *       └─ if buffer == "time":
 *           └─ time() → localtime() → strftime()  ← get current time string
 *           └─ sendto(time_str, client_addr)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
        char* ip = "127.0.0.1";
        int port = 8776;
        int server_sock;
        char buffer[1024];
        struct sockaddr_in server_addr, client_addr;
        socklen_t addr_size = sizeof(client_addr);

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

        printf("UDP time server listening on port %d\n", port);

        while (1) {
                int n = recvfrom(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_size);
                buffer[n] = '\0';
                printf("Received request: %s\n", buffer);

                if (strcmp(buffer, "time") == 0) {
                        time_t t = time(NULL);
                        struct tm* tm_info = localtime(&t);
                        char time_str[1024];
                        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
                        sendto(server_sock, time_str, strlen(time_str), 0, (struct sockaddr*)&client_addr, addr_size);
                        printf("Sent time: %s\n", time_str);
                }
        }

        close(server_sock);
}
