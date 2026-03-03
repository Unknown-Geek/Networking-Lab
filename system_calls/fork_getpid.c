/*
 * fork() - Print process IDs
 * ---------------------------
 * fork()
 *   ├─ child  (pid==0): getpid()  ← own PID
 *   │                   getppid() ← parent's PID
 *   └─ parent (pid>0) : wait(NULL)
 *                         getpid()  ← own PID
 *                         getppid() ← its parent's PID (shell)
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
		printf("Process ID : %d\n",getpid());
		printf("Parent process ID : %d\n",getppid());
	} else {
		wait(NULL);
		printf("Process ID : %d\n",getpid());
		printf("Parent process ID : %d\n",getppid());
	}
}