#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define PACKET_COUNT 3

int main() {
        srand(time(0));
        int packets[PACKET_COUNT], i, rate, bucket_size, time_to_transmit, clk, output;
        int remaining_size = 0;

        for (int i = 0; i < PACKET_COUNT; i++)
                packets[i] = (rand() % 6 + 1) * 10;

        printf("Enter output rate: ");
        scanf("%d", &rate);
        printf("Enter bucket size: ");
        scanf("%d", &bucket_size);
        printf("\n");

        i = 0;
        while (i < PACKET_COUNT || remaining_size > 0) {
                if (i < PACKET_COUNT) {
                        if ((packets[i] + remaining_size) > bucket_size)
                                printf("Bucket overflow! Packet %d dropped\n", packets[i]);
                        else {
                                remaining_size += packets[i];
                                printf("Incoming packet size: %d\n", packets[i]);
                                printf("Bytes remaining to transmit: %d\n\n", remaining_size);
                        }
                        i++;
                }

                time_to_transmit = (rand() % 4 + 1) * 10;
                printf("Time left for transmission: %d units\n\n", time_to_transmit);

                for (clk = 10; clk <= time_to_transmit; clk += 10) {
                        sleep(1);
                        if (remaining_size) {
                                if (remaining_size <= rate) {
                                        output = remaining_size;
                                        remaining_size = 0;
                                } else {
                                        output = rate;
                                        remaining_size -= rate;
                                }
                                printf("Packet %d transmitted\n", output);
                                printf("Bytes remaining to transmit: %d\n\n", remaining_size);
                        } else {
                                printf("No packets to transmit!\n\n");
                        }
                }
        }
}