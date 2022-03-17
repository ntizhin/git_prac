#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mod1.h"
#include "mod2.h"
#include "mod3.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

void print_p(pid_t *arr, int n){
	printf("n = %d\n", n);
	int i;
	for(i = 0; i < n; i++) printf("pid[%d] = %d\n", i, arr[i]);
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
		printf("Can't open file.\n");
		return 0;
	}	
	
	// General part //
	char *str;
	char **arr_word;
	int n;
	signal(SIGINT, SIG_IGN);

	// bgrd
	int count_bgrd = 0; 
	pid_t *pid_bgrd_array = (pid_t *)malloc(0);
	// bgrd

	while(!feof(f1)){
		if(count_bgrd > 0) pid_bgrd_array = check_bgrd(pid_bgrd_array, &count_bgrd);
	//	print_p(pid_bgrd_array, count_bgrd);
		printf(">");
		str = EntrStr(f1);
		arr_word = PrStr(str, &n);
		if(n > 0) comand_shell(arr_word, &pid_bgrd_array, &count_bgrd);
        free(str);
		for(i = 0; i < n; i++) free(arr_word[i]);
		free(arr_word);
	}
	free(pid_bgrd_array);
	fclose(f1);
	fclose(f2); 
	return 0;
}
