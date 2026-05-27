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
    int index;
    int client_sock;
    socklen_t client_size;
    struct sockaddr_in client_addr;
} Client;

char *ip = "127.0.0.1";
int server_sock;
socklen_t server_size;
struct sockaddr_in server_addr;
int port = 4533;
int count = 0;

Client clients[SIZE];
pthread_t client_thread[SIZE];

void *client_communication(void *arg) {
    char buffer[1024];
    char message[1024];
    int index = clients[count].index;
    int client_sock = clients[count].client_sock;
    socklen_t client_size = clients[count].client_size;
    struct sockaddr_in client_addr = clients[count].client_addr;

    while(1) {
        bzero(buffer,1024);
        recv(client_sock,buffer,sizeof(buffer),0);

        if(strcmp(buffer,"SEND") == 0) {
            bzero(buffer,1024);
            recv(client_sock,buffer,sizeof(buffer),0);

            bzero(message,1024);
            sprintf(message,"Client %d : %s",index,buffer);
            printf("%s\n",message);

            for(int i=0 ; i<count ; i++) {
                if (index == i)
                    continue;
                else 
                    send(clients[i].client_sock,message,sizeof(message),0);
            }
        }
    }
}

int main() {
    char buffer[1024];
    int n;

    server_sock = socket(AF_INET,SOCK_STREAM,0);
    if(server_sock < 0) {
        printf("Socket Error.\n");
        exit(1);
    }

    server_size = sizeof(server_addr);

    memset(&server_addr,'\0', server_size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(server_sock,(struct sockaddr *) &server_addr, server_size);
    if(n < 0) {
        printf("Bind Error.\n");
        exit(1);
    }

    printf("Server running on port %d.\n",port);

    listen(server_sock,SIZE);

    while(1) {
        clients[count].client_size = sizeof(clients[count].client_addr);
        clients[count].client_sock = accept(server_sock,(struct sockaddr *)& clients[count].client_addr, &clients[count].client_size);
        clients[count].index = count;
        pthread_create(&client_thread[count],NULL,client_communication,NULL);
        printf("Client %d connected.\n",count);
        count++;
    }
    close(server_sock);
}