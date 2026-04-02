#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <pthread.h>

#define TIMEOUT 5

int main() {
	char *ip = "127.0.0.100";
	int port = 6666;

	int sockfd;
	struct sockaddr_in addr;
	char buffer[1024];
	socklen_t addr_size;

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd<0) {
		perror("Socket Error\n");
		exit(1);
	}

	printf("UDP server socket created.\n");
	memset(&addr,'\0',sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);

	int Counter = 1;

	while (Counter<=3) {
		addr_size = sizeof(addr);

		bzero(buffer,1024);
		sprintf(buffer,"%d",Counter);
		printf("Client: Sending packet %s\n",buffer);
		sendto(sockfd,buffer,1024,0,(struct sockaddr*)&addr,addr_size);

		struct timeval timeout ;
		timeout.tv_sec = TIMEOUT ;
		timeout.tv_usec = 0;

		if(setsockopt(sockfd , SOL_SOCKET , SO_RCVTIMEO ,&timeout , sizeof(timeout)) < 0)
			printf ( "Client: Error \n" ) ;

		else {
			bzero(buffer,1024);
			int rec = recvfrom (sockfd,buffer,1024,0,(struct sockaddr*) &addr,&addr_size);
			
			if(rec<0) {
				printf("Client: Timeout error! Resending packet!\n");
			} else {
				printf ( "Client : Received acknowledgement for packet %s\n ",buffer);
				Counter++;
			}	
		}	
		sleep(1);
	}
	close(sockfd);
}
