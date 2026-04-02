#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>

#define PORT 8776

int main(){
	int sock_fd;
	char buffer[1024];
	struct sockaddr_in serv_addr;	

	if((sock_fd = socket(AF_INET, SOCK_DGRAM, 0))<0){
		printf("ERROR CREATING SOCKET");
		return 1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if(inet_aton("127.0.0.1", &serv_addr.sin_addr) == 0) {
		printf("INVALID SERVER IP");
		return 1;
	}

	strcpy(buffer, "time");

	if(sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0) {
		printf("SEND FAILED");
		return 1;
	}
	printf("Sent time request to server..\n");
	int n = recvfrom(sock_fd, buffer, 1024, 0, NULL, NULL);

	if(n<0){
		printf("RECIEVE FAILED");
		return 1;
	}
	buffer[n] ='\0';
	printf("Server time: %s \n", buffer);


	close(sock_fd);
	return 0;
}
