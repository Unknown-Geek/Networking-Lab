/*
 * UDP Server
 * ----------
 * socket(AF_INET, SOCK_DGRAM)   ← connectionless, no listen/accept
 *   └─ bind(server_addr)
 *       └─ recvfrom(buffer, &client_addr)  ← also captures sender's addr
 *       └─ sendto(buffer, client_addr)     ← reply using captured addr
 *       └─ close()
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
	char* ip = "127.0.0.1";
        int port =5431;
        socklen_t addr_size;
        int server_sock;
	struct sockaddr_in server_addr,client_addr;
        char buffer[1024];
        int n;
        
	server_sock = socket(AF_INET,SOCK_DGRAM,0);
        if(server_sock<0){
                perror("Socket error");
                exit(0);
        }
        
	memset(&server_addr,'\0',sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port =htons(port);
        server_addr.sin_addr.s_addr = inet_addr(ip);
        
	n = bind(server_sock,(struct sockaddr*)&server_addr,sizeof(server_addr));
        if(n<0){
                perror("Bind error");
                exit(1);
        }
	
	printf("Server listening on port: %d\n",port);
        bzero(buffer,1024);
        
	addr_size = sizeof(client_addr);
        recvfrom(server_sock,buffer,1024,0,(struct sockaddr*)&client_addr,&addr_size);
        printf("Data from client : %s\n",buffer);
        
	strcpy(buffer,"Data received");
        printf("Data to client : %s\n",buffer);
        sendto(server_sock,buffer,1024,0,(struct sockaddr*)&client_addr,sizeof(client_addr));

	close(server_sock);
}
