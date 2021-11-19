#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv){
	int fd[2];
	pipe(fd);
	int pid;
	if((pid = fork()) == 0){
		dup2(fd[1], 1);
		close(fd[1]);
		close(fd[0]);
		execl("prog1.c", "prog1.c", argv[0], argv[1], 0);
		fprintf(stderr, "error: execl() did't end\n");
		return 1;
	}
	if(pid == -1){
		fprintf(stderr, "Error: process 1 didn't create\n");
		return 1;
	}
	
	if((pid = fork()) == 0){
		dup2(fd[0], 0);
		close(fd[1]);
		close(fd[0]);
		execl("prog2.c", "prog2.c", argv[2], argv[3], 0);
		fprintf(stderr, "error: execl() did't end\n");
		return 2;
	}
	if(pid == -1){
		fprintf(stderr, "Error: process 1 didn't create\n");
		return 1;
	}
	int status;
	while((pid = wait(&status)) != -1){
		if(status) fprintf(stderr, "error: pid = %d; status = %x\n", pid, status);;
	}
	return 0;
}
