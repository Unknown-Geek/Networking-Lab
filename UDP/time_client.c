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
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    char *ip = "127.0.0.1";
    int port = 4534;
    char buffer[1024];
    struct sockaddr_in server_addr;
    int client_sock;
    socklen_t addr_size;

    client_sock = socket(AF_INET,SOCK_DGRAM,0);
    if(client_sock < 0) {
        printf("Error creating socket\n");
    }

    memset(&server_addr,'\0',sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    addr_size = sizeof(server_addr); 

    bzero(buffer,1024);
    strcpy(buffer,"TIME");
    sendto(client_sock,buffer,1024,0,(struct sockaddr*)&server_addr,addr_size);
    printf("Time request sent to server : %s\n",buffer);

    bzero(buffer,1024);
    recvfrom(client_sock,buffer,1024,0,(struct sockaddr*)&server_addr,&addr_size);
    printf("Data received : %s\n",buffer);

    close(client_sock);
}