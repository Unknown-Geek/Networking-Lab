#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define TIMEOUT 5
#define packetsCount 10
#define windowSize 3

typedef struct Packet {
    int id;
    int sent;
} Packet;

Packet packets[packetsCount];
pthread_t thread[packetsCount], receiveThread;

int sockfd;
struct sockaddr_in addr;

int windowStart = 0;
int windowEnd = 0;
int nextToSend = 0;

void* sendPacket(void* arg)
{
    Packet* packet = (Packet*)arg;
    char buffer[1024];

    while (packet->sent == 0)
    {
        printf("Client: Sending packet %d\n", packet->id);

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%d", packet->id);

        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr*)&addr, sizeof(addr));

        sleep(TIMEOUT);

        if (packet->sent == 0)
            printf("Client: Timeout for packet %d\n", packet->id);
    }

    return NULL;
}

void* receivePacket(void* arg __attribute__((unused)))
{
    socklen_t addr_size = sizeof(addr);
    char buffer[1024];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr*)&addr, &addr_size);

        if (n <= 0)
            continue;

        buffer[n] = '\0';

        char msg[20];
        int packetID;

        sscanf(buffer, "%s %d", msg, &packetID);

        if (packetID < 1 || packetID > packetsCount)
            continue;

        if (strcmp(msg, "NACK") == 0)
        {
            printf("Client: Received NACK for packet %d. Resending...\n", packetID);
            packets[packetID - 1].sent = 0;
        }
        else if (strcmp(msg, "ACK") == 0)
        {
            printf("Client: Received ACK for packet %d\n", packetID);
            packets[packetID - 1].sent = 1;

            if (windowStart == packetID - 1)
            {
                while (windowStart < packetsCount &&
                       packets[windowStart].sent == 1)
                {
                    windowStart++;

                    if (windowEnd < packetsCount - 1)
                        windowEnd++;
                }
            }

            if (windowStart >= packetsCount)
                break;
        }
        else
        {
            printf("Client: Invalid message received\n");
        }
    }

    return NULL;
}

int main()
{
    for (int i = 0; i < packetsCount; i++)
    {
        packets[i].id = i + 1;
        packets[i].sent = 0;
    }

    char* ip = "127.0.0.100";
    int port = 6665;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Socket Error");
        exit(1);
    }

    printf("UDP client socket created.\n");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    pthread_create(&receiveThread, NULL, receivePacket, NULL);

    windowStart = 0;
    windowEnd = windowSize - 1;
    nextToSend = 0;

    // Send initial window
    for (int i = windowStart; i <= windowEnd && i < packetsCount; i++)
    {
        pthread_create(&thread[i], NULL, sendPacket, &packets[i]);
        nextToSend++;
    }

    // Sliding window control
    while (windowStart < packetsCount)
    {
        if (nextToSend < packetsCount && nextToSend <= windowEnd)
        {
            pthread_create(&thread[nextToSend], NULL, sendPacket,
                           &packets[nextToSend]);
            nextToSend++;
        }

        sleep(1);
    }

    printf("All packets acknowledged.\n");

    close(sockfd);
    return 0;
}
