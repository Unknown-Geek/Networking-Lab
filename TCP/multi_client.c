/*
 * TCP Multi-Client Chat Client
 * ----------------------------
 * socket() → connect(server_addr)
 *   └─ pthread_create(server_communication)  ← recv loop in background
 *   └─ main loop: scanf("SEND")
 *       └─ send("SEND") → send(target_id) → send(message)
 *
 * server_communication(thread):
 *   └─ loop: recv(message) → print
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

void *server_communication(void *arg) {
        int client_sock = *((int*)arg);
        char buffer[1024];
        while (1) {
                bzero(buffer, 1024);
                int n = recv(client_sock, buffer, 1024, 0);
                buffer[n] = '\0';
                printf("%s\n", buffer);
        }
        return NULL;
}

int main() {
        int port = 5123;
        int client_sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
                exit(0);
        printf("Connection established\n");

        pthread_t thread;
        pthread_create(&thread, NULL, server_communication, (void*)&client_sock);

        char input[1024];
        while (1) {
                scanf("%s", input);
                if (strcmp(input, "SEND") == 0) {
                        send(client_sock, input, 1024, 0);
                        scanf("%s", input);
                        send(client_sock, input, 1024, 0);
                        scanf("%[^\n]", input);
                        send(client_sock, input, 1024, 0);
                }
        }
}
