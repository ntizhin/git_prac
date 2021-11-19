#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

//pr1 && pr2 && pr3 ....
int main(int argc, char **argV){
	int i;
	for (i=1; i<argc; i++){
		int status, pid;
		if(fork()){
			if(pid == -1){
				fprintf(stderr, "error: fork() for pr%d\n", i);
				return 1;
			}
			pid = wait(&status);
			if(status) return 0;
		}
		else{
			execlp(argV[i], argV[i], 0);
			fprintf(stderr, "error: execl() for pr%d\n", i);
			return 2;
		}
	}
	return 0;	
}
