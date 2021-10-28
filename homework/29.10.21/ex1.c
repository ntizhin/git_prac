#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argV){
	int i;
	for (i = 1; i < argc; i++){
		int status, pid;
		if(fork() > 0){
			pid = wait(&status);
			continue;
		}
			execl(argV[i], argV[i], 0);
			return -1;
		}
	return 0;
}
