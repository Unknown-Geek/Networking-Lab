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
	struct sockaddr_in client_addr;
        char buffer[1024];
        socklen_t addr_size;

        client_sock = socket(AF_INET,SOCK_STREAM,0);
        if(client_sock<0){
                perror("Socket Error!\n");
                exit(1);
        }
       
       	memset(&client_addr,'\0',sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(port);

	connect(client_sock, (struct sockaddr*)&client_addr, sizeof(client_addr));

        bzero(buffer,1024);
        printf("Enter the data:");
        scanf("%[^\n]",buffer);
        printf("Client Data to server : %s\n",buffer);
        send(client_sock, buffer, strlen(buffer), 0);
        
	bzero(buffer,1024);
        addr_size = sizeof(client_addr);
        recv(client_sock, buffer, sizeof(buffer), 0);
        printf("Data from server : %s\n",buffer);
}
