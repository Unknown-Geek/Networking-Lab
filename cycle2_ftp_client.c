#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SIZE 1024

int main() {
        char* ip = "127.0.0.1";
        int client_sock;
        struct sockaddr_in server_addr;

        client_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (client_sock < 0) {
                perror("Socket error");
                exit(1);
        }

        memset(&server_addr, '\0', sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(6666);
        server_addr.sin_addr.s_addr = inet_addr(ip);
        connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

        char buffer[SIZE];
        char filename[100];
        FILE* fptr;
        int choice;

        while (1) {
                printf("\n1. Upload a file (PUT)\n2. Download a file (GET)\n3. Exit (BYE)\nEnter choice: ");
                scanf("%d", &choice);
                switch (choice) {
                        case 1:
                                bzero(buffer, SIZE);
                                strcpy(buffer, "PUT");
                                printf("Enter filename: ");
                                scanf("%s", filename);
                                fptr = fopen(filename, "r");
                                if (!fptr) {
                                        perror("File not found");
                                } else {
                                        send(client_sock, buffer, SIZE, 0);
                                        bzero(buffer, SIZE);
                                        strcpy(buffer, filename);
                                        send(client_sock, buffer, SIZE, 0);
                                        while (!feof(fptr)) {
                                                bzero(buffer, SIZE);
                                                fscanf(fptr, "%[^\n]", buffer);
                                                send(client_sock, buffer, SIZE, 0);
                                        }
                                        strcpy(buffer, "END$");
                                        send(client_sock, buffer, SIZE, 0);
                                        fclose(fptr);
                                        printf("File %s sent\n", filename);
                                }
                                break;
                        case 2:
                                bzero(buffer, SIZE);
                                strcpy(buffer, "GET");
                                send(client_sock, buffer, SIZE, 0);
                                bzero(buffer, SIZE);
                                recv(client_sock, buffer, SIZE, 0);
                                printf("Server PID: %s\n", buffer);
                                printf("Enter filename: ");
                                scanf("%s", filename);
                                bzero(buffer, SIZE);
                                strcpy(buffer, filename);
                                send(client_sock, buffer, SIZE, 0);
                                bzero(buffer, SIZE);
                                recv(client_sock, buffer, SIZE, 0);
                                if (strcmp(buffer, "404") == 0) {
                                        printf("File not found on server\n");
                                } else if (strcmp(buffer, "200") == 0) {
                                        fptr = fopen(filename, "w");
                                        while (1) {
                                                bzero(buffer, SIZE);
                                                recv(client_sock, buffer, SIZE, 0);
                                                if (strcmp(buffer, "END$") == 0) break;
                                                fprintf(fptr, "%s", buffer);
                                        }
                                        fclose(fptr);
                                        printf("File %s received\n", filename);
                                }
                                break;
                        case 3:
                                bzero(buffer, SIZE);
                                strcpy(buffer, "BYE");
                                send(client_sock, buffer, SIZE, 0);
                                close(client_sock);
                                printf("Exiting\n");
                                exit(0);
                }
        }
}