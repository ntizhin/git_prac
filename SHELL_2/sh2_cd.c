#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mod1.h"
#include <sys/wait.h>

void process(char **arr){
	int pid;
	int status;
	if((pid = fork()) == 0){   // child
		execvp(arr[0], arr);
		fprintf(stderr,"error: exec did't work\n");
		exit(1);
	}
	// parrent 
	if(pid == -1) fprintf(stderr, "error: process did't start\n");   
	else{	
		wait(&status);
		if(status) fprintf(stderr, "error: process did't end, status = %x\n", status); 
	}
}

void cd(char **arr, int arg){
    if(arg > 2) fprintf(stderr, "too many arguments");
    else if(arg == 1) chdir(getenv("HOME"));
    else if(chdir(arr[1]) == -1) fprintf(stderr, "error: The directory didn't change\n");
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
		str = EntrStr(f1);
		arr_word = PrStr(str, &n);
		if(n > 0){
			if(!strcmp(arr_word[0], "cd")) cd(arr_word, n);
			else process(arr_word);
		}
        free(str);
		for(i = 0; i < n; i++) free(arr_word[i]);
		free(arr_word);
	}
	fclose(f1);
	fclose(f2); 
	return 0;
}