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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

void *send_data(void *arg) {
    char buffer[1024];
    int len;
    int client_sock = *((int *)arg);

    while(1){
        bzero(buffer,1024);
        gets(buffer);
        if(strcmp(buffer,"SEND") == 0){
            send(client_sock,buffer,sizeof(buffer),0);
            bzero(buffer,1024);
            printf("Enter data to send : ");
            gets(buffer);
            send(client_sock,buffer,sizeof(buffer),0);
        }
    }
}

void *receive_data(void *arg) {
    char buffer[1024];
    int len;
    int client_sock = *((int *)arg);

    while(1){
        bzero(buffer,1024);
        len = recv(client_sock,buffer,sizeof(buffer),0);
        if(len > 0) {
            printf("%s\n",buffer);
        }
    }
}

int main() {
    char *ip = "127.0.0.1";
    int port = 4536;
    struct sockaddr_in server_addr;
    int client_sock;
    socklen_t client_size,server_size;
    int len;
    char buffer[1024];

    pthread_t send_thread,receive_thread;

    client_sock = socket(AF_INET,SOCK_STREAM,0);
    if(client_sock < 0) {
        printf("Error creating socket\n");
    }

    memset(&server_addr,'\0',sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    server_size = sizeof(server_addr);
    connect(client_sock,(struct sockaddr*)&server_addr,server_size);

    pthread_create(&send_thread,NULL,send_data,&client_sock);
    pthread_create(&receive_thread,NULL,receive_data,&client_sock);

    pthread_join(send_thread,NULL);
    pthread_join(receive_thread,NULL);

    return 0;
}