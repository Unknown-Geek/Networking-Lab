#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PACKET_COUNT 10

typedef struct {
        int id;
        int received;
} Packet;

Packet packets[PACKET_COUNT];

int main() {
        for (int i = 0; i < PACKET_COUNT; i++) {
                packets[i].id = i + 1;
                packets[i].received = 0;
        }

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

        while (1) {
                bzero(buffer, 1024);
                addr_size = sizeof(client_addr);
                recvfrom(server_sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addr_size);
                sleep(1);

                int id = atoi(buffer);
                if (packets[id - 1].received == 1 || rand() % 5 == 0)
                        continue;
                else if (rand() % 4 == 0) {
                        printf("Server: Corrupted packet %s. Sending NACK\n", buffer);
                        sprintf(buffer, "NACK %d", id);
                        sendto(server_sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, addr_size);
                } else {
                        printf("Server: Received packet %s. Sending ACK\n", buffer);
                        sprintf(buffer, "ACK %d", id);
                        sendto(server_sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, addr_size);
                        packets[id - 1].received = 1;
                }
        }
        close(server_sock);
}
