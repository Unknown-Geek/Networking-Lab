#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/wait.h>

#define PORT 8776

int main(){
	pid_t pid;
	int sock_fd, n;
	char buffer[1024];
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t cli_len = sizeof(cli_addr);
	

	if((sock_fd = socket(AF_INET, SOCK_DGRAM, 0))<0){
		printf("ERROR CREATING SOCKET");
		return 1;
	}


	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	if(bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
		printf("ERROR BINDING THE SOCKET");
		return 1;
	}
	printf("UDP time server is running.. \n");

	while(1){
		n = recvfrom(sock_fd, buffer, 1024, 0, (struct sockaddr *)&cli_addr, &cli_len);
		if(n<0){
			printf("ERROR RECIEVING DATA");
			return 1;
		}
		buffer[n] = '\0';
		printf("RECEIVED CLIENT REQUESTS: %s\n", buffer);
		if((pid = fork()) <0){
			printf("ERROR FORKING");
			return 1;
		}else if(pid == 0){
			if(strcmp (buffer, "time")==0){					
				time_t t = time(NULL);
				struct tm * tm_info = localtime(&t);
				char time_str [1024];
				strftime(time_str, 1024, "%Y-%m-%d %H:%M:%S", tm_info);
				if(sendto(sock_fd, time_str, strlen(time_str), 0, (struct sockaddr *)&cli_addr, cli_len) <0) {
					printf("ERROR SENDING DATA");
						return 1;
					}else{
						printf("Sent current system time to the client: %s\n",time_str);
					}
			}
			exit(0);
		}
	}
	
	close(sock_fd);
	return 0;

}
