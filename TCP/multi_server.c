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
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#define SIZE 5

typedef struct {
    struct sockaddr_in client_addr;
    socklen_t client_size;
    int client_sock;
    int index;
}Client;

Client clients[SIZE];
pthread_t thread[SIZE];

void *client_communication(void *arg) {
    Client *c = (Client*) arg;
    struct sockaddr_in sockaddr = c->client_addr;
    int client_sock = c->client_sock;
    int client_size = c->client_size;
    int index = c->index;
    char buffer[1024];
    int len;

    printf("Client %d connected.\n",index+1);

    while(1) {
        bzero(buffer,1024);
        len = recv(client_sock,buffer,sizeof(buffer),0);
        buffer[len] = '\0';
        
        if(strcmp(buffer,"SEND") == 0) {
            len = recv(client_sock,buffer,sizeof(buffer),0);
            buffer[len] = '\0';
            printf("Data Received from Client %d: %s\n",index+1,buffer);
            for(int i=0 ; i<SIZE ; i++) {
                if(i != index)
                    send(clients[i].client_sock,buffer,sizeof(buffer),0);
            }
        }
    }
}

int main() {
    char *ip = "127.0.0.1";
    int port = 4536;
    char buffer[1024];
    struct sockaddr_in server_addr;
    int server_sock;
    int n;
    int count = 0;
    socklen_t server_size;

    server_sock = socket(AF_INET,SOCK_STREAM,0);
    if(server_sock < 0) {
        printf("Error creating socket\n");
    }

    memset(&server_addr,'\0',sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    server_size = sizeof(server_addr);
    
    n = bind(server_sock,(struct sockaddr*)&server_addr,server_size);
    if(n < 0) {
        printf("Bind error\n");
    }

    printf("Server listening on port %d\n",port);
    listen(server_sock,SIZE);

    while(1) {
        clients[count].client_size = sizeof(clients[count].client_addr);
        clients[count].client_sock = accept(server_sock,(struct sockaddr*)&clients[count].client_addr,&clients[count].client_size);
        clients[count].index = count;
        pthread_create(&thread[count],NULL,client_communication,&clients[count]);
        count++;
    }

    return 0;
}