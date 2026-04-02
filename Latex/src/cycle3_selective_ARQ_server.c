#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define packetsCount 10

typedef struct Packet {
	int id;
	int received;
}Packet;

Packet packets[packetsCount];


int main() {
	//initialize struct
	for(int i=0 ; i<packetsCount ; i++) {
		packets[i].id = i+1;
		packets[i].received = 0;
	}

	char *ip = "127.0.0.100";
	int port=6665;
	srand(time(0));

	int sockfd;
	struct sockaddr_in server_addr,client_addr;
	char buffer[1024];
	socklen_t client_size,server_size;
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

	while(1) {
		bzero(buffer,1024);
		client_size = sizeof(client_addr);
		recvfrom(sockfd,buffer,1024,0,(struct sockaddr*)&client_addr,&client_size);
		
		sleep(1);

		int packet = atoi(buffer);
		if(packets[packet-1].received == 1 || rand() % 5 == 0) {
			continue;
		}

		else if(rand() % 4 == 0) {
			printf("Server: Received corrupted packet %s.Sending negative acknowledgement.\n",buffer);
			sprintf(buffer,"NACK %d",packet);
			sendto(sockfd,buffer,1024,0,(struct sockaddr*)&client_addr,client_size);
		}

		else { 
			printf("Server: Received packet %s. Sending acknowledgement\n",buffer);
			sprintf(buffer,"ACK %d",packet);
			sendto(sockfd,buffer,1024,0,(struct sockaddr*)&client_addr,client_size);
			packets[packet-1].received = 1;
		}
	}
	close(sockfd);
}
