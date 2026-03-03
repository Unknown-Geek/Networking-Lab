#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
	int n;
	int flag = 0;

	printf("Enter the number : ");
	scanf("%d",&n);

	pid_t pid = fork();
	if (pid < 0) {
		printf("Fork Failed\n");
		return 1;
	}

	if (pid > 0) {
		for(int i=2 ; i<n/2 ; i++) {
			if (n % i == 0){
				flag = 1;
				break	;
			}
		}

		if (flag == 1) {
			printf("%d is not a prime number\n",n);
		} else {
			printf("%d is a prime number\n",n);
		}
	}
}