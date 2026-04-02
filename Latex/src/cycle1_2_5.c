#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
	pid_t pid = fork();
	if (pid<0) {
		printf("Fork Failed\n");
		return 1;
	}

	if (pid == 0) {
		for(int i=0 ; i<5 ; i++){
			printf("Hello\n");
			sleep(1);
		}
	} else {
		wait(NULL);
	}
}