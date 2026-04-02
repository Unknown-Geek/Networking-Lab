#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define packetsCount 3

int main() {
	srand(time(0));
	int packets[packetsCount], i, rate, bucketSize, timetoTransmit, clk,op;
	int remainingSize = 0;

	for(int i=0 ; i<packetsCount ; i++) 
		packets[i] = (rand()%6 + 1) * 10;

	printf("\nEnter the Output Rate : ");
	scanf("%d",&rate);
	printf("Enter the Bucket Size : ");
	scanf("%d",&bucketSize);
	printf("\n");

	i=0;
	while(i<packetsCount || remainingSize>0) {
		if(i<packetsCount) {
			if((packets[i] + remainingSize) > bucketSize)
				printf("Bucket capacity exceeded! Packet %d overflow\n",packets[i]);
			else {
				remainingSize += packets[i];
				printf("Incoming packet size : %d\n",packets[i]);
				printf("Bytes remaining to transmit : %d\n\n",remainingSize);
			}
			++i;
		}

		timetoTransmit = (rand() % 4 + 1) * 10;
		printf("Time left for transmission : %d units\n\n",timetoTransmit);

		for(clk = 10; clk<=timetoTransmit; clk+=10) {
			sleep(1);
			if(remainingSize) {
				if (remainingSize <= rate) {
					op = remainingSize;
					remainingSize = 0;
				} else {
					op = rate;
					remainingSize -= rate;
				}

				printf("Packet %d transmitted\n",op);
				printf("Bytes remaining to transmit : %d\n\n",remainingSize);
			} else {
				printf("Time left for transmission : %d units\n",timetoTransmit);
				printf("No packets to transmit!\n\n");
			}
		}
	}
}