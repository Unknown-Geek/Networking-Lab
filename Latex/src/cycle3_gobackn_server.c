#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int main() {
	char *ip = "127.0.0.100";
	int port=6665;
	srand(time(0));

	int sockfd;
	struct sockaddr_in server_addr,client_addr;
	char buffer[1024];
	socklen_t client_size,server_size;
	int n,b;
	
	int recd[10];
	for(int i=0;i<10;i++) 
		recd[i] = -1;

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd<0) {
		perror("Socket Error\n");
		exit(1);
	}

	printf("UDP server socket created.\n");
	memset(&server_addr,'\0',sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip);

	server_size = sizeof(server_addr);
	n = bind(sockfd,(struct sockaddr*)&server_addr,server_size);
	if(n < 0) {
		perror("Bind Error\n");
		exit(1);
	}

	while(1) {
		bzero(buffer,1024);
		client_size = sizeof(client_addr);
		recvfrom(sockfd,buffer,1024,0,(struct sockaddr*)&client_addr,&client_size);
		
		b=atoi(buffer);
		if(recd[b] != b) {
			recd[b] = b;
			printf("Server: Received packet %s\n",buffer);
		}
		else
			printf("Server: Received packet %s(duplicate) \n",buffer);

		sleep(1);

		if(rand() % 3 == 0) {
			printf("Server: Sending ACK to client for packet %s\n",buffer);
			sendto(sockfd,buffer,1024,0,(struct sockaddr*)&client_addr,client_size);
		}
	}
	close(sockfd);
}
