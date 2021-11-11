#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argC, char **argV){
	int fd[2];
	pipe(fd);
	int pid;
	
	//pr1
	if((pid = fork()) == 0){  
		dup2(fd[1], 1);
		close(fd[0]);
		close(fd[1]);
		execl(argV[1], argV[1], argV[2], argV[3], 0);
		fprintf(stderr, "error: execl() for pr1");
		return 1;
	}
	if(pid == -1){
		fprintf(stderr, "error: fork() for pr1");
		return 2;
	}
	//pr1
	
	//pr2	
	if((pid = fork()) == 0){  
		dup2(fd[0], 0);
		close(fd[0]);
		close(fd[1]);
		execl(argV[4], argV[4], 0);
		fprintf(stderr, "error: execl() for pr2");
		return 3;
	}
	if(pid == -1){
		fprintf(stderr, "error: fork() for pr2");
		return 4;
	}
	//pr2
	
	close(fd[0]);
	close(fd[1]);
	int status;
	while((pid = wait(&status)) != -1){
		if(status){
			fprintf(stderr, "The process didn't end, status = %x\n", status);
			return 5;
		}
	}
	//pr3
	int f;
	if((f = open(argV[6],O_WRONLY | O_APPEND | O_CREAT)) == -1){
		fprintf(stderr, "Can't open file\n");
		return 6;
	}
	else{
		dup2(f, 1);
		close(f);
		execl(argV[5], argV[5],  0);
		fprintf(stderr, "error: execl() for pr3");
		return 7;
	}
}
	
