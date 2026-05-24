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
#include <unistd.h>
#include <time.h>

#define PACKETS 10

int main() {
    int bucket_size, rate;
    int packets[PACKETS];
    int remaining_size = 0;
    int sent;

    srand(time(0));

    for(int i=0 ; i<PACKETS ; i++) {
        packets[i] = (rand() % 6 + 1) * 10; 
    }
    printf("Enter the bucket_size : ");
    scanf("%d",&bucket_size);

    printf("Enter the output rate : ");
    scanf("%d", &rate);
    printf("\n");
    
    for (int i=0 ; i<PACKETS ; i++) {
        if (packets[i] + remaining_size > bucket_size) {
            printf("Bucket Overflow. Packet %d dropped\n\n",packets[i]);
        } else {
            remaining_size += packets[i];
            printf("Incoming packet : %d. Bucket : %d\n\n",packets[i],remaining_size);
        }

        sleep(1);
        if(remaining_size < rate) 
            sent = remaining_size;
        else
            sent = rate;
        
        remaining_size -= sent;

        printf("Transmitted : %d. Bucket : %d\n\n",sent,remaining_size);
    }
}