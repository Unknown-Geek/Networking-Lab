#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define TIMEOUT_VALUE 12 
#define PACKET_COUNT 5 
#define WINDOW_SIZE 2 

void sendWindowPackets(int *sockfd, struct sockaddr_in addr, int packets[], int windowStart, int windowEnd) {
	char buffer[1024];
	while (windowStart < windowEnd) {
		bzero(buffer, 1024);
		sprintf(buffer, "%d", packets[windowStart]);
		printf("Client: Sending packet %s\n", buffer);
		sendto(*sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, sizeof(addr));
		windowStart++;
	}
}

int main() {
	char *ip = "127.0.0.100";
	int port = 6665;
	int sockfd;
	struct sockaddr_in addr;
	char buffer[1024];

	socklen_t addr_size;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("Socket error");
		exit(1);
	}

	printf("UDP server socket created.\n");
	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);

	int packets[PACKET_COUNT];
	for (int i = 0; i < PACKET_COUNT; ++i) {
		packets[i] = i + 1;
	}

	int windowStart = 0;
	int windowEnd = windowStart + WINDOW_SIZE;
	int flag = 1;
	sendWindowPackets(&sockfd, addr, packets, windowStart, windowEnd);

	while (windowStart != PACKET_COUNT) {
		bzero(buffer, 1024);
		addr_size = sizeof(addr);

		struct timeval timeout;
		timeout.tv_sec = TIMEOUT_VALUE;
		timeout.tv_usec = 0;

		if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		printf("Error setting socket timeout\n");
		} else {
			int rec = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, &addr_size);
			if (rec < 0) {
				printf("Client: Timeout error! Sending window packets again\n");
				sendWindowPackets(&sockfd, addr, packets, windowStart, windowEnd);
			} else {
				printf("Client: Received acknowledgment for packet %s\n", buffer);
				if (atoi(buffer) != packets[windowStart]) {
					printf("Client: Wrong acknowledgment! Sending window packets again\n");
					sendWindowPackets(&sockfd, addr, packets, windowStart, windowEnd);
				} else {
					windowStart++;
					if (windowEnd < PACKET_COUNT) {
						windowEnd++;
					}

					if (windowStart != PACKET_COUNT && flag == 1) {
						bzero(buffer, 1024);
						sprintf(buffer, "%d", packets[windowEnd - 1]);
						printf("Client: Sending packet %s\n", buffer);
						sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, sizeof(addr));
					}

					if (windowEnd == PACKET_COUNT) {
						flag = 0;
					}
				}
			}
		}
	sleep(1); 
	}
}
