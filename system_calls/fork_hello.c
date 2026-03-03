/*
 * fork() - Child prints repeatedly, parent waits
 * ------------------------------------------------
 * fork()
 *   ├─ child  (pid==0): loop 5x: print "Hello" → sleep(1)
 *   └─ parent (pid>0) : wait(NULL)   ← waits for child to finish
 */
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