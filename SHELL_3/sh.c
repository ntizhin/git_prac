#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mod1.h"
#include "mod3.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

int process(char **arr, int n){
	int pid;
	int status;
	// discriptor
	int fd[2] = {-1, -1};
	if(!(arr = pr_io(arr, &n, fd))) return 1;
	if((pid = fork()) == 0){   // child
		if(fd[0] != -1){
			dup2(fd[0], 0);
			close(fd[0]);
		}
		if(fd[1] != -1){ 
			dup2(fd[1] , 1);
			close(fd[1] );
		}	
		char ***pr = malloc(0);
		int n = 0;
		while(*arr){
			n += 1;
			pr = realloc(pr, n * (sizeof(char**)));
			pr[n - 1] = arr;
			for(; (*arr) && strcmp(*arr, "|"); arr++);
			if(*arr){
				free(*arr);
				*arr = NULL;     
				arr++;
			}
		}
		pipeline(pr, n);
		exit(0);
	}
	// parrent 
	close(fd[0]);
	close(fd[1]);
	if(pid == -1) fprintf(stderr, "error: process did't start\n");   
	else{	
		wait(&status);
		if(status) fprintf(stderr, "error: process did't end, status = %d\n", status); 
	}
	return 0;
}

int main(int agc, char *argV[]){
	int i;

	// INSTALL FILE //
	FILE *f1, *f2;	
	f1 = stdin;
	f2 = stdout;
	for(i = 0; i < agc; i++){
		if(!strcmp(argV[i], "-i")) f1 = fopen(argV[i + 1], "r");
		if(!strcmp(argV[i], "-o")) f2 = fopen(argV[i + 1], "w");
	};
	if((f1 == NULL) | (f2 == NULL)){
		printf("Cannot open file.\n");
		return 0;
	}	
	
	// General part //
	char *str;
	char **arr_word;
	int n;
	while(!feof(f1)){
		printf(">");
		str = EntrStr(f1);
		arr_word = PrStr(str, &n);
		if(n > 0) process(arr_word, n);
        free(str);
		for(i = 0; i < n; i++) free(arr_word[i]);
		free(arr_word);
	}
	fclose(f1);
	fclose(f2); 
	return 0;
}
