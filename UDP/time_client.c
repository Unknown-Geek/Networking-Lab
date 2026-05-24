/*
 * UDP Time Client
 * ---------------
 * socket(SOCK_DGRAM)
 *   └─ sendto("time", server_addr)  ← request current time
 *   └─ recvfrom(buffer)             ← receive time string
 *   └─ print buffer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>

int main() {
    char *ip = "127.0.0.1";
    int client_sock,server_sock;
    socklen_t client_size,server_size;
    struct sockaddr_in client_addr,server_addr;
    int port = 4532;
    int n;
    char buffer[1024];

    client_sock = socket(AF_INET,SOCK_DGRAM,0);
    if(client_sock < 0) {
        printf("Socket error.\n");
        exit(1);
    }

    client_size = sizeof(client_addr);
    server_size = sizeof(server_addr);

    memset(&server_addr,'\0',server_size); // memset always server_addr
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    bzero(buffer,1024);
    strcpy(buffer,"TIME");

    // sendto & recvfrom -> if sock is client then addr of server and vice-versa (always different)
    sendto(client_sock,buffer,sizeof(buffer),0,(struct sockaddr *)&server_addr,server_size);
    printf("Client : Request sent to server.\n");
    
    bzero(buffer,1024);
    recvfrom(client_sock,buffer,sizeof(buffer),0,(struct sockaddr *)&server_addr,&server_size);
    printf("Client : Response from server : %s\n",buffer);

    close(client_sock);
}