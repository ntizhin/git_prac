#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argV){
	int i;
	int n1 = 0, n2 = 0, n3 = 0;
	for (i = 1; i < argc; i++){
		int f;
		if(0 == (f = fork())){
			execl(argV[i], argV[i], 0);
			exit(1);
		}
		else if(-1 == f) n3 += 1;  
	}
	int status, pid;
	while((pid = wait(&status)) != -1){
		if(!status) n1 += 1;
		else{
			if(status & 0xFF) n3 += 1;
			else n2 += 1;
		}
	}
	printf("%d %d %d\n", n1, n2, n3);
	return 0;
}
