/*
 * FTP Server (TCP)
 * ----------------
 * socket() → bind() → listen() → accept()
 *   └─ loop: recv(command)
 *       ├─ "PUT": recv(filename) → fopen("w")
 *       │         └─ loop: recv(chunk) until "END$" → fprintf → fclose
 *       ├─ "GET": send(pid) → recv(filename) → fopen("r")
 *       │         └─ send("200") → loop: fgets+send until EOF → send("END$")
 *       │         or send("404") if file not found
 *       └─ "BYE": close() → exit()
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SIZE 1024

int main() {
        char* ip = "127.0.0.1";
        int server_sock, client_sock;
        struct sockaddr_in server_addr, client_addr;
        socklen_t addr_size;

        server_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (server_sock < 0) {
                perror("Socket error");
                exit(1);
        }

        memset(&server_addr, '\0', sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(6666);
        server_addr.sin_addr.s_addr = inet_addr(ip);

        if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                perror("Bind error");
                exit(1);
        }

        listen(server_sock, SIZE);
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        printf("Client connected\n");

        char buffer[SIZE];
        char filename[100];
        FILE* fptr;

        while (1) {
                bzero(buffer, SIZE);
                recv(client_sock, buffer, sizeof(buffer), 0);

                if (strcmp(buffer, "PUT") == 0) {
                        bzero(buffer, SIZE);
                        recv(client_sock, buffer, sizeof(buffer), 0);
                        strcpy(filename, buffer);
                        fptr = fopen(filename, "w");
                        while (1) {
                                bzero(buffer, SIZE);
                                recv(client_sock, buffer, sizeof(buffer), 0);
                                if (strcmp(buffer, "END$") == 0) break;
                                fprintf(fptr, "%s", buffer);
                        }
                        fclose(fptr);
                        printf("File %s received\n", filename);

                } else if (strcmp(buffer, "GET") == 0) {
                        bzero(buffer, SIZE);
                        sprintf(buffer, "%d", getpid());
                        send(client_sock, buffer, SIZE, 0);

                        bzero(buffer, SIZE);
                        recv(client_sock, buffer, sizeof(buffer), 0);
                        strcpy(filename, buffer);

                        fptr = fopen(filename, "r");
                        if (!fptr) {
                                printf("File %s not found\n", filename);
                                strcpy(buffer, "404");
                                send(client_sock, buffer, SIZE, 0);
                        } else {
                                strcpy(buffer, "200");
                                send(client_sock, buffer, SIZE, 0);
                                while (!feof(fptr)) {
                                        bzero(buffer, SIZE);
                                        fgets(buffer, sizeof(buffer), fptr);
                                        send(client_sock, buffer, SIZE, 0);
                                }
                                strcpy(buffer, "END$");
                                send(client_sock, buffer, SIZE, 0);
                                fclose(fptr);
                                printf("File %s sent\n", filename);
                        }

                } else if (strcmp(buffer, "BYE") == 0) {
                        close(server_sock);
                        exit(0);
                }
        }
}
