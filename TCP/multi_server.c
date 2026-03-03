/*
 * TCP Multi-Client Chat Server
 * ----------------------------
 * socket() → setsockopt(SO_REUSEADDR) → bind() → listen()
 *   └─ loop: accept()  ← new client
 *       └─ pthread_create(client_communication)
 *
 * client_communication(thread per client):
 *   └─ loop: recv("SEND")
 *       └─ recv(target_id)
 *       └─ recv(message)
 *       └─ forward message to all other clients via send()
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 1024

int client_count = 0;

typedef struct {
        int index;
        int client_sock;
        struct sockaddr_in client_addr;
        int addr_size;
} Client;

Client clients[MAX_CLIENTS];
pthread_t thread[MAX_CLIENTS];

void *client_communication(void *arg) {
        Client *c = (Client*)arg;
        int client_sock = c->client_sock;
        int index = c->index;
        char buffer[1024];

        printf("Client %d connected\n", index + 1);

        while (1) {
                bzero(buffer, 1024);
                int n = recv(client_sock, buffer, 1024, 0);
                buffer[n] = '\0';

                if (strcmp(buffer, "SEND") == 0) {
                        n = recv(client_sock, buffer, 1024, 0);
                        buffer[n] = '\0';
                        int id = atoi(buffer) - 1;
                        n = recv(client_sock, buffer, 1024, 0);
                        buffer[n] = '\0';
                        for (int i = 0; i < client_count; i++) {
                                if (i != id)
                                        send(clients[i].client_sock, buffer, 1024, 0);
                        }
                }
        }
        return NULL;
}

int main() {
        int port = 5123;
        int server_sock = socket(AF_INET, SOCK_STREAM, 0);

        int opt = 1;
        setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
                perror("Bind error");
                exit(0);
        }

        listen(server_sock, MAX_CLIENTS);
        printf("Server listening on port %d\n", port);

        while (1) {
                socklen_t addr_size = sizeof(clients[client_count].client_addr);
                clients[client_count].client_sock = accept(server_sock, (struct sockaddr*)&clients[client_count].client_addr, &addr_size);
                clients[client_count].index = client_count;
                pthread_create(&thread[client_count], NULL, client_communication, (void*)&clients[client_count]);
                client_count++;
        }
        return 0;
}
