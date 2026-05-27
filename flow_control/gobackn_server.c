/*
 * Go-Back-N Server (receiver)
 * ----------------------------
 * socket(SOCK_DGRAM) → bind()
 *   └─ loop:
 *       ├─ recvfrom(packet)
 *       ├─ check recd[] array for duplicates
 *       ├─ sleep(1)
 *       └─ rand()%3==0 → sendto(ACK)   ← randomly drop ACKs
 *                   else → drop  (client resends entire window)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define PACKET_COUNT 5

int main() {
    char *ip = "127.0.0.1";
    int port = 4555;
    int server_sock;
    struct sockaddr_in client_addr,server_addr;
    socklen_t client_size,server_size;

    char buffer[1024];
    int n;
    int id;
    int recd[PACKET_COUNT];
    srand(time(NULL));

    server_sock = socket(AF_INET,SOCK_DGRAM,0);
    if(server_sock < 0) {
        printf("Socket Error.\n");
        exit(1);
    }

    client_size = sizeof(client_addr);
    server_size = sizeof(server_addr);

    memset(&server_addr,'\0',server_size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(server_sock,(struct sockaddr *)&server_addr,server_size);
    if(n < 0) {
        printf("Bind Error.\n");
        exit(1);
    }

    printf("Server listening on port %d\n", port);

    for(int i=0 ; i<PACKET_COUNT ; i++) 
        recd[i] = -1;

    while(1) {
        bzero(buffer,1024);
        recvfrom(server_sock,buffer,sizeof(buffer),0,(struct sockaddr *)&client_addr,&client_size);
        id = atoi(buffer);

        if(recd[id] == id) {
            printf("Server : Duplicate packet received.\n");
        } else {
            recd[id] = id;
            printf("Server : Received packet %d.\n",id);
        }

        sleep(1);

        if(rand() % 3 == 0) {
            printf("Server : Sending acknowledgement for packet %d\n",id);
            sendto(server_sock,buffer,sizeof(buffer),0,(struct sockaddr *)&client_addr,client_size);
        }
    }
    close(server_sock);
}