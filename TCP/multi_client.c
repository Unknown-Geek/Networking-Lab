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
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

void *send_data(void *arg){
    int client_sock = *((int *)arg);
    char buffer[1024];

    bzero(buffer,1024);
    while(1) {
        gets(buffer);
        buffer[strlen(buffer)] = '\0'; //Attach null character of each message to be sent

        if(strcmp(buffer,"SEND") == 0) {
            send(client_sock,buffer,strlen(buffer)+1,0);

            bzero(buffer,1024);
            printf("Enter data to send : ");
            gets(buffer);
            buffer[strlen(buffer)] = '\0';
            send(client_sock,buffer,strlen(buffer)+1,0);
        }
    }
}

void *receive_data(void *arg){
    int client_sock = *((int *)arg);
    char buffer[1024];
    int len;

    bzero(buffer,1024);
    while(1) {
        len = recv(client_sock,buffer,sizeof(buffer),0);
        if (len>0) 
            printf("%s\n",buffer);
        bzero(buffer,1024);
    }
}

int main() {
    char *ip = "127.0.0.1";
    socklen_t server_size;
    struct sockaddr_in server_addr;
    int client_sock;
    int port = 4533;
    char buffer[1024];
    pthread_t send_thread,receive_thread;

    client_sock = socket(AF_INET,SOCK_STREAM,0);
    if (client_sock < 0) {
        printf("Socket error.\n");
        exit(1);
    }

    server_size = sizeof(server_addr);

    memset(&server_addr,'\0',server_size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    connect(client_sock,(struct sockaddr *)&server_addr, server_size);

    pthread_create(&send_thread,NULL,send_data,&client_sock);
    pthread_create(&receive_thread,NULL,receive_data,&client_sock);

    pthread_join(send_thread,NULL);
    pthread_join(receive_thread,NULL);

    close(client_sock);
}