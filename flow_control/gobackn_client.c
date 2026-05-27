/*
 * Go-Back-N Client (sender)
 * --------------------------
 * socket(SOCK_DGRAM)
 *   └─ sendWindowPackets(windowStart, windowEnd)  ← send entire window at once
 *   └─ loop until all packets ACKed:
 *       ├─ setsockopt(SO_RCVTIMEO) → recvfrom()
 *       ├─ timeout / wrong ACK → resend entire window (go back N)
 *       └─ correct ACK → windowStart++ , windowEnd++, send new packet
 *
 * sendWindowPackets(): loop from windowStart to windowEnd → sendto each
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define PACKET_COUNT 5
#define TIMEOUT 5
#define WINDOWSIZE 2

char *ip = "127.0.0.1";
int port = 4555;
int client_sock;
struct sockaddr_in server_addr;
socklen_t server_size;
int packets[PACKET_COUNT];

void send_packet(int packet) {
    char buffer[1024];
    bzero(buffer,1024);
    sprintf(buffer,"%d",packet);
    printf("Client : Sending packet %d\n",packet);
    sendto(client_sock,buffer,sizeof(buffer),0,(struct sockaddr *)&server_addr, server_size);
}

void send_window(int start,int end) {
    while(start < end){
        send_packet(packets[start]);
        start++;
    }
}

int main() {
    char buffer[1024];
    struct timeval timeout;
    server_size = sizeof(server_addr);

    client_sock = socket(AF_INET,SOCK_DGRAM,0);
    if (client_sock < 0) {
        printf("Socket Error.\n");
        exit(1);
    }

    memset(&server_addr,'\0',server_size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    setsockopt(client_sock,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    
    for(int i=0 ; i<PACKET_COUNT ; i++) {
        packets[i] = i;
    }

    int window_start = 0;
    int window_end = WINDOWSIZE;

    send_window(window_start,window_end);

    while(window_start < PACKET_COUNT) {
        bzero(buffer,1024);
        int n = recvfrom(client_sock,buffer,sizeof(buffer),0,(struct sockaddr *)&server_addr,&server_size);

        if(n < 0) {
            printf("Client : Timed Out! Resending window.\n");
            send_window(window_start,window_end);
        } else { // n > 0 
            int ack_id = atoi(buffer);
            if(ack_id != window_start) {    // Resend window
                printf("Wrong acknowledgement! Resending window.\n");
                send_window(window_start,window_end);
            } else {
                printf("Client : Received acknowledgement for packet %d\n",ack_id);
                window_start++;
                if(window_end < PACKET_COUNT) {     // Send next packet
                    send_packet(packets[window_end]);
                    window_end++;
                }
            }
        }
    }
    close(client_sock);
}