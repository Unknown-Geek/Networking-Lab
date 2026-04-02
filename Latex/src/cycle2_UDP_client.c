#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
	char* ip = "127.0.0.1";
        int port =5431;
        struct sockaddr_in addr;
        char buffer[1024];
        socklen_t addr_size;
        int client_sock;
        
	client_sock = socket(AF_INET,SOCK_DGRAM,0);
        if(client_sock<0){
                perror("Socket Error!\n");
                exit(1);
        }

        memset(&addr,'\0',sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        
	bzero(buffer,1024);
        printf("Enter the data:");
        scanf("%[^\n]",buffer);
        printf("Client Data to server : %s\n",buffer);
        sendto(client_sock,buffer,1024,0,(struct sockaddr*)&addr,sizeof(addr));
        
	bzero(buffer,1024);
        addr_size = sizeof(addr);
        recvfrom(client_sock,buffer,1024,0,(struct sockaddr*)&addr,&addr_size);
        printf("Data from server : %s\n",buffer);
}
