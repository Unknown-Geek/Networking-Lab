/*
 * UDP Client
 * ----------
 * socket(AF_INET, SOCK_DGRAM)   ← no connect() needed
 *   └─ sendto(buffer, server_addr)   ← send with destination addr each time
 *   └─ recvfrom(buffer, &server_addr)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
        char* ip = "127.0.0.1";
        int port = 5431;
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

        bzero(buffer, 1024);
        printf("Enter the data: ");
        scanf("%[^\n]", buffer);
        printf("Client data to server: %s\n", buffer);
        sendto(client_sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

        bzero(buffer, 1024);
        addr_size = sizeof(server_addr);
        recvfrom(client_sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, &addr_size);
        printf("Data from server: %s\n", buffer);
}
