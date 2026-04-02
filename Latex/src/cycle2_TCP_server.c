#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
        char* ip = "127.0.0.1";
        int port =4321;
        socklen_t addr_size;
        int client_sock,server_sock,n;
        struct sockaddr_in server_addr,client_addr;
        char buffer[1024];

        server_sock = socket(AF_INET,SOCK_STREAM,0);
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
        listen(server_sock, 10);

        client_sock = accept(server_sock,(struct sockaddr*)&client_addr,&addr_size);

        bzero(buffer,1024);
        recv(client_sock, buffer, sizeof(buffer), 0);
        printf("Data from client : %s\n",buffer);
        
        strcpy(buffer,"Data received");
        printf("Data to client : %s\n",buffer);
        send(client_sock, buffer, strlen(buffer), 0);
        close(server_sock);
}
