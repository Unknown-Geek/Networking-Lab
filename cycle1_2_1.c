#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
	int num[100],n;
	int even_sum=0;
	int odd_sum=0;

	printf("Enter value of n : ");
	scanf("%d",&n);
	printf("Enter the numbers : \n");
	for(int i=0 ; i<n ; i++){
		scanf("%d",&num[i]);
	}

	pid_t pid = fork();
	if (pid<0) {
		printf("Fork Failed\n");
		return 1;
	}

	if (pid == 0) {
		for(int i=0 ; i<n ; i++) {
			if (num[i] % 2 != 0){
				odd_sum += num[i];
			}
		}
		printf("Child Process : Odd Sum = %d\n",odd_sum);
	} else {
		for(int i=0 ; i<n ; i++) {
			if (num[i] % 2 == 0){
				even_sum += num[i];
			}
		}
		printf("Parent Process : Even Sum = %d\n",even_sum);
	}
}