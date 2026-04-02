#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main() {
	char *ip = "127.0.0.100";
	int port=6666;

	int sockfd;
	int packetCount = 0;
	struct sockaddr_in server_addr;
	char buffer[1024];
	socklen_t server_size;
	int n;

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
	if(n>0) {
		perror("Bind Error\n");
		exit(1);
	}

	while(packetCount <= 10) {
		server_size = sizeof(server_addr);
		bzero(buffer,1024);
		sprintf(buffer,"%d",packetCount);
		printf("Client : Sending packet %d\n",packetCount);
		sendto(sockfd,buffer,1024,0,(struct sockaddr*)&server_addr,server_size);

		bzero(buffer,1024);
		recvfrom(sockfd,buffer,1024,0,(struct sockaddr*)&server_addr,&server_size);
		while(1) {
			sleep(1);
			if(atoi(buffer) == packetCount) {
				printf("Client : Received ACK from server for packet %d\n",packetCount);
				break;
			}
		}

		printf("Client : Sending ACK packet %d\n",packetCount);
		sendto(sockfd,buffer,1024,0,(struct sockaddr*)&server_addr,server_size);

		packetCount++;
	}
	close(sockfd);
}