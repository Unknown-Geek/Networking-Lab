/*
 * UDP Time Server
 * ---------------
 * socket(SOCK_DGRAM) → bind()
 *   └─ loop: recvfrom(buffer)   ← wait for any request
 *       └─ if buffer == "time":
 *           └─ time() → localtime() → strftime()  ← get current time string
 *           └─ sendto(time_str, client_addr)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>

int main() {
    char *ip = "127.0.0.1";
    socklen_t client_size,server_size;
    struct sockaddr_in client_addr,server_addr;
    int server_sock;
    int port = 4532;
    int n;
    char buffer[1024];

    server_sock = socket(AF_INET,SOCK_DGRAM,0);
    if( server_sock < 0) {
        printf("Socket Error\n");
        exit(1);
    }

    client_size = sizeof(client_addr);
    server_size = sizeof(server_addr);

    memset(&server_addr,'\0',server_size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(server_sock,(struct sockaddr *)&server_addr,server_size);
    if (n < 0) {
        printf("Bind error\n");
        exit(1);
    }

    printf("Server running on port %d\n",port);

    bzero(buffer,1024);
    recvfrom(server_sock,buffer,sizeof(buffer),0,(struct sockaddr *)&client_addr , &client_size);

    if(strcmp(buffer,"TIME") == 0) {
        printf("Server : Request received from the client.\n");
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        char time_str[1024];
        strftime(time_str,sizeof(time_str),"%p %F",tm);

        sendto(server_sock,time_str,sizeof(time_str),0,(struct sockaddr *)&client_addr, client_size);
        printf("Server : Response sent to client : %s\n",time_str);
    }

    close(server_sock);
}


