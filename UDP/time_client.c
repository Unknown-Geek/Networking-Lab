#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
        char* ip = "127.0.0.1";
        int port = 8776;
        int client_sock;
        char buffer[1024];
        struct sockaddr_in server_addr;
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

        strcpy(buffer, "time");
        sendto(client_sock, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        printf("Sent time request to server\n");

        bzero(buffer, 1024);
        addr_size = sizeof(server_addr);
        int n = recvfrom(client_sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &addr_size);
        buffer[n] = '\0';
        printf("Server time: %s\n", buffer);

        close(client_sock);
}
