/*
 * Leaky Bucket Algorithm
 * -----------------------
 * Input: output rate, bucket_size
 *   └─ generate random packets
 *   └─ loop while packets remain or bucket not empty:
 *       ├─ add packet to bucket:
 *       │   ├─ if packet+remaining > bucket_size → overflow (drop)
 *       │   └─ else → remaining += packet size
 *       ├─ pick random transmission time
 *       └─ every clock tick (sleep 1s):
 *           └─ transmit min(remaining, rate) bytes → remaining -= rate
 *
 * Concept: output is smoothed to a constant rate regardless of bursty input
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define PACKET_COUNT 3

int main() {
        srand(time(0));
        int packets[PACKET_COUNT], i, rate, bucket_size;
        int remaining_size = 0;

        for (int i = 0; i < PACKET_COUNT; i++)
                packets[i] = (rand() % 6 + 1) * 10;

        printf("Enter output rate: ");
        scanf("%d", &rate);
        printf("Enter bucket size: ");
        scanf("%d", &bucket_size);
        printf("\n");

        for (int i = 0; i < PACKET_COUNT || remaining_size > 0; i++) {
                if (i < PACKET_COUNT) {
                        if (packets[i] + remaining_size > bucket_size)
                                printf("Bucket overflow! Packet %d dropped\n", packets[i]);
                        else {
                                remaining_size += packets[i];
                                printf("Incoming packet size: %d, bucket: %d\n", packets[i], remaining_size);
                        }
                }

                sleep(1);
                int sent;
                if (remaining_size < rate)
                        sent = remaining_size;
                else
                        sent = rate;
                        
                remaining_size -= sent;
                printf("Transmitted: %d, bucket: %d\n\n", sent, remaining_size);
        }
}
