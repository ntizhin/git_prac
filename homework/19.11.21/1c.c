#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>


int main(int argC, char **argV){
	int pid;
	if((pid = fork()) == 0){ //pr1
		int i;
		for(i = 2; i < argC; i++){
			int fd[2];
			int child;
			pipe(fd);
			if((child = fork()) == 0){
				dup2(fd[0], 0);
				close(fd[0]);
				close(fd[1]);
			}
			else{
				if(child == -1){
					fprintf(stderr, "Error: process %d didn't create\n", i);
					return 1;
				}
				dup2(fd[1], 1);
				close(fd[0]);
				close(fd[1]);
				execlp(argV[i - 1], argV[i - 1], 0);
				fprintf(stderr, "Error: exec in process %d didn't end\n", i - 1);
				return 2;
			}
		}
		execlp(argV[argC - 1], argV[argC - 1], 0);
		fprintf(stderr, "Error: exec in process %d didn't end\n", argC - 1);
		return 2;
	}
	if(pid == -1){
		fprintf(stderr, "Error: process 1 didn't create\n");
		return 1;
	}
	else{
		int status;
		wait(&status);
	}
	return 0;
}
