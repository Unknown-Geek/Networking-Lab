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
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define SIZE 5

typedef struct{
    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t client_size;
    int index;
} Client;

Client clients[SIZE];
pthread_t client_thread[SIZE];
int count = 0;

void *client_communication(void *arg) {
    Client *c = (Client *) arg;
    int index = c->index;
    socklen_t client_size = c->client_size;
    struct sockaddr_in client_addr = c->client_addr;
    int client_sock = c->client_sock;

    char buffer[1024];
    char message[1024];
    bzero(buffer,1024);
    bzero(message,1024);

    while(1) {
        int len = recv(client_sock,buffer,sizeof(buffer),0);
        buffer[len] = '\0';
        if(strcmp(buffer,"SEND") == 0) {
            bzero(buffer,1024);
            bzero(message,1024);

            len = recv(client_sock,buffer,sizeof(buffer),0);
            buffer[len] = '\0';
            for(int i=0 ; i<count ; i++) {
                if (i == index)
                    continue;
                else {
                    sprintf(message,"Client %d : %s",index,buffer);
                    printf("%s\n",message);
                    message[strlen(message)] = '\0';
                    send(clients[i].client_sock,message,strlen(message),0);
                } 
            }
        }
    }
}

int main() {
    char *ip = "127.0.0.1";
    socklen_t server_size;
    struct sockaddr_in server_addr;
    int client_sock,server_sock;
    int port = 4533;
    char buffer[1024];
    int n;

    server_sock = socket(AF_INET,SOCK_STREAM,0);
    if(server_sock < 0) {
        printf("Socket Error.\n");
        exit(1);
    }

    server_size = sizeof(server_addr);

    memset(&server_addr,'\0',server_size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(server_sock,(struct sockaddr *)&server_addr, server_size);
    if(n < 0) {
        printf("Bind Error.\n");
        exit(1);
    }

    printf("Server running on port %d\n",port);
    listen(server_sock,SIZE);

    while(1) {
        clients[count].client_size = sizeof(clients[count].client_addr);
        clients[count].client_sock = accept(server_sock,(struct sockaddr *)&clients[count].client_addr, &clients[count].client_size);
        clients[count].index = count;
        pthread_create(&client_thread[count],NULL,client_communication,&clients[count]);
        printf("Client %d connected.\n",count);
        count++;
    }

    close(server_sock);
}