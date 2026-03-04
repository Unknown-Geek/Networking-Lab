/*
 * TCP Client
 * ----------
 * socket(AF_INET, SOCK_STREAM)
 *   └─ connect(server_addr)  ← no bind needed
 *       └─ send(buffer)   ← send to server
 *       └─ recv(buffer)   ← read reply
 *       └─ close()
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
	char* ip = "127.0.0.1";
        int port =4321;
        int client_sock;
	struct sockaddr_in server_addr;
        char buffer[1024];
        socklen_t addr_size;

        client_sock = socket(AF_INET,SOCK_STREAM,0);
        if(client_sock<0){
                perror("Socket Error!\n");
                exit(1);
        }
       
       	memset(&server_addr,'\0',sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = inet_addr(ip);

	connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

        bzero(buffer,1024);
        printf("Enter the data:");
        scanf("%[^\n]",buffer);
        printf("Client Data to server : %s\n",buffer);
        send(client_sock, buffer, strlen(buffer), 0);
        
	bzero(buffer,1024);
        addr_size = sizeof(server_addr);
        recv(client_sock, buffer, sizeof(buffer), 0);
        printf("Data from server : %s\n",buffer);
}
