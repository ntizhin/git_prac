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
	if(n == 0){
		perror("Error: n = 0\n");
		exit(1);
	}
	
	
	int child, i;
	pid_t pid_parrent = getpid();
	int fd[2];
	int f0 = -1, f1;
	
	//creating processes and channels
	for(i = 0; i < n; i++){
		pipe(fd);
		if((child = fork()) == 0){
			f0 = fd[0];
			close(fd[1]);
		}
		else{ 
			if(child == -1){
				fprintf(stderr, "Error: process %d didn't create\n", i);
				exit(2);
			}
			f1 = fd[1];
			close(fd[0]);
			break;
		}
	}
	// file processing
	if(pid_parrent == getpid()){
		int f = open(argV[1], O_RDONLY);
		if(f == -1){
			perror("Cannot open file.\n");
			exit(3);
		}	
		int m, c;
		while((m = read(f, &c, sizeof(int)))){
			if(m == -1){
				perror("Error: read = -1\n");
				exit(4);
			}
			if(m != sizeof(int)){
				perror("Error: broken byte\n");
				exit(5);
			}
			write(f1, &c, sizeof(int));
		} 
		close(f1);
		while(flag) pause();
	}
	else{
		int sum = 0, c;
		while(read(f0, &c, sizeof(int))){
			if(c % n == i) sum += c;
			else if(i != n) write(f1, &c, sizeof(int));
		}
		printf("sum = %d; pr = %d\n", sum, i);
		if(i != n) close(f1);
		else kill(pid_parrent, SIGUSR1);
	}
	return 0;
}
