#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

int main() {
	char str[100];
	int len;

	printf("Enter the string : ");
	gets(str);
	len = strlen(str);

	pid_t pid = fork();
	if (pid<0) {
		printf("Fork Failed\n");
		return 1;
	}

	if (pid == 0) {
		for(int i=0 ; i<len ; i++) {
			if(str[i] != ' '){
				printf("%c",str[i]-32);
			} else {
				printf("%c",str[i]);
			}
		}
		printf("\n");
	}
	else {
		wait(NULL);
	}
}