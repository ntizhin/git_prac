#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

int flag = 1;
void SigHndlr(int s){
	flag = 0;
}

int main(int argc, char **argV){
	signal(SIGUSR1, SigHndlr);
	int n = atoi(argV[2]);
	int child_arr[n];
	int child, i;
	for(i = 0; i < n; i++){
		if((child = fork()) == 0){
			int f1 = open(argV[1], O_RDONLY);
			if(f1 == -1){
				printf("Cannot open file.\n");
				return 0;
			}	
			lseek(f1, i * sizeof(int), SEEK_SET);
			int c, m, sum = 0;
			for(; (m = read(f1, &c, sizeof(int)));){
				if(m == -1){
					fprintf(stderr, "Error: read = -1\n");
					return 1;
				}
				sum += c;
				lseek(f1, (n - 1) * sizeof(int), SEEK_CUR);
			} 
			for(;flag;);
			printf("sum = %d n = %d\n", sum, i);
			return 0;
		}
		if(child == -1){
			fprintf(stderr, "Error: process %d didn't create\n", i);
			return 1;
		}
		child_arr[i] = child;
	}
	for(i = 0; i < n; i++){
		kill(child_arr[i], SIGUSR1);
		int status;
		wait(&status);
	}
	return 0;
}
