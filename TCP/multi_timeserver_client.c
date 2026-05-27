#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

char *ip = "127.0.0.1";
int client_sock;
socklen_t server_size;
struct sockaddr_in server_addr;
int port = 4535;

void *send_data(void *arg) {
    char buffer[1024];

    while(1) {
        bzero(buffer,1024);
        gets(buffer);

        if(strcmp(buffer,"TIME") == 0){    
            buffer[strlen(buffer)] = '\0';
            send(client_sock,buffer,sizeof(buffer),0);
        }
    }
}

void *receive_data(void *arg) {
    char buffer[1024];
    int n=0;

    while(1) {
        bzero(buffer,1024);
        n = recv(client_sock,buffer,sizeof(buffer),0);
        if(n > 0) {
            printf("%s\n",buffer);
        }
    }
}

int main() {
    char buffer[1024];
    pthread_t send_thread,receive_thread;

    server_size = sizeof(server_addr);

    client_sock = socket(AF_INET,SOCK_STREAM,0);
    if(client_sock < 0) {
        printf("Socket Error.\n");
        exit(1);
    }

    memset(&server_addr,'\0', server_size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    connect(client_sock,(struct sockaddr *) &server_addr, server_size);
    
    pthread_create(&send_thread,NULL,send_data,NULL);
    pthread_create(&receive_thread,NULL,receive_data,NULL);

    pthread_join(send_thread,NULL);
    pthread_join(receive_thread,NULL);

    close(client_sock);
}