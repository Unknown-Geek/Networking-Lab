/*
 * File I/O using fopen/fprintf/fscanf
 * -------------------------------------
 * fopen("sample.txt", "w")   ← open for writing
 *   └─ gets(line) → fprintf(fp, line)
 *   └─ fclose()
 * fopen("sample.txt", "r")   ← reopen for reading
 *   └─ fscanf(fp, line) → printf(line)
 *   └─ fclose()
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	FILE *fp;
	char line[100];
	fp = fopen("sample.txt","w");

	printf("Write a line to file : \n");
	gets(line);
	fprintf(fp,"%s",line);

	printf("Reading from file : \n");
	fscanf(fp,"%s",line);
	printf("%s",line);	
	printf("\n");
	fclose(fp);

	return 0;
}