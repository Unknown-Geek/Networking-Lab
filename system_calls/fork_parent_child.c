/*
 * fork() - Basic parent/child with wait()
 * -----------------------------------------
 * fork()
 *   ├─ child  (pid==0): print "Child"
 *   └─ parent (pid>0) : wait()  ← blocks until child exits
 *                         print "Parent"
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
		printf("Child\n");
	} else {
		wait();
		printf("Parent\n");
	}
}