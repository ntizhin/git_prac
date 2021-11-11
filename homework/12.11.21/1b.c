#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

int main(int argC, char **argV){
	int fd[2];
	pipe(fd);
	int pid;
	if((pid = fork()) == 0){  // pr1
		int f1;
		if((f1 = open(argV[2],O_RDONLY)) == -1){
			fprintf(stderr, "Can't open file\n");
			return 1;
		}
		dup2(f1, 0);
		close(f1);
		dup2(fd[1], 1);
		close(fd[0]);
		close(fd[1]);
		execl(argV[1], argV[1], 0);
		fprintf(stderr, "error: execl() for pr1");
		return 2;
	}
	if(pid == -1){
		fprintf(stderr, "error: fork() for pr1");
		return 3;
	}
		
	if((pid = fork()) == 0){  // pr2
		int f2;
		if((f2 = open(argV[4],O_WRONLY | O_CREAT)) == -1){
			fprintf(stderr, "Can't open file\n");
			return 4;
		}
		dup2(f2, 1);
		close(f2);
		dup2(fd[0], 0); 
		close(fd[0]);
		close(fd[1]);
		execl(argV[3], argV[3], 0);
		fprintf(stderr,"error: execl() for pr2");
		return 5;
	}
	if(pid == -1){
		fprintf(stderr, "error: fork() for pr2");
		return 6;
	}
	
	close(fd[0]);
	close(fd[1]);
	
	int status;
	while((pid = wait(&status)) != -1){
		if(status){
			fprintf(stderr, "The process didn't end, status = %x\n", status);
			return 7;
		}
	}
	
	//pr3
	execl(argV[5], argV[5], 0); 
	fprintf(stderr, "error: execl() for pr3");
	return 8;
}

	
