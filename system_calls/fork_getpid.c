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
		printf("Process ID : %d\n",getpid());
		printf("Parent process ID : %d\n",getppid());
	} else {
		wait(NULL);
		printf("Process ID : %d\n",getpid());
		printf("Parent process ID : %d\n",getppid());
	}
}