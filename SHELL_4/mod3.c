#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mod1.h"
#include "mod2.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

void add_pid_arr(pid_t pid, pid_t **pid_array, int n){
    *pid_array = realloc(*pid_array, (n + 1) * sizeof(pid_t*));
    (*pid_array)[n] = pid;
}

pid_t *check_bgrd(pid_t *pid_array, int *n){
    int i, size = *n, status;
	pid_t *new_arr = malloc(0); 
	int j = 0;
    for(i = 0; i < size; i++){
        if(waitpid(pid_array[i], &status, WNOHANG) > 0){
            printf("[%d] Done; status = %d\n", pid_array[i], status);
        } 
		else{
			j += 1;
			new_arr = realloc(new_arr, j * sizeof(pid_t));
			new_arr[j - 1] = pid_array[i];
		}
	}
	free(pid_array);
	*n = j;
	return new_arr;
}

int comand(char **arr, int n){
	int pid;
	int status;
	// discriptor
	int fd[2] = {-1, -1};
	if(!(arr = pr_io(arr, &n, fd))) return 1;
	if((pid = fork()) == 0){   // child
		signal(SIGINT, SIG_DFL);
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
		exit(pipeline(pr, n) >> 8);
	}
	// parrent 
	close(fd[0]);
	close(fd[1]);
	if(pid == -1) fprintf(stderr, "error: process did't start\n");   
	else{
		waitpid(pid, &status, 0);
		if(status) fprintf(stderr, "error: process did't end, status = %x\n", status); 
	}
	return status;
}

void comand_if(char **arr){
		char **cmd;
        int n, status;
        enum {dis, con, end} reason;
        while(*arr){
            n = 0;
            cmd = arr;
            for(; *arr && strcmp(*arr, "&&") && strcmp(*arr, "||"); arr++, n++);
            if(*arr){
                if(!strcmp(*arr, "&&")) reason = con;
                else reason = dis;
                free(*arr);
                *arr = NULL;
                arr++;
            }
            else reason = end;
            status = comand(cmd, n);
            if((reason != end) && (((reason == con) && status) || ((reason == dis) && !status))) break;
        }
}

void bgrd_process(char **arr, pid_t **pid_bgrd_array, int *c_bgrd){
	int f;
	int pid = fork();
	switch (pid)
	{
	case -1:
		perror("Error: fork() from bgrd didn't work\n");
		break;
	case 0:
		if((f = open("/dev/null", O_RDONLY | O_WRONLY | O_TRUNC | O_CREAT)) == -1){
			perror("Can't open file for bgrd_process\n");
			exit(1);
		}
		dup2(f, 0);
		dup2(f, 1);
		close(f);
		setpgrp();
		printf("getppid = %d group = %d\n", getpid(), getpgrp());
		comand_if(arr);
		exit(0);
	default:
		add_pid_arr(pid, pid_bgrd_array, *c_bgrd);
		*c_bgrd += 1;
	//	printf("n = %d\n", *c_bgrd);
		printf("[%d] %d\n", *c_bgrd, pid);
		break;
	}
}

void comand_shell(char **arr, pid_t **pid_bgrd_array, int *c_bgrd){
	char **cmd;
    enum {bgrd, sml, end} reason;
    while(*arr){
        cmd = arr;
        for(; *arr && strcmp(*arr, "&") && strcmp(*arr, ";"); arr++);
        if(!*arr) reason = end;
        else if(!strcmp(*arr, "&")) reason = bgrd;
        else reason = sml;
        switch (reason){
			case bgrd:
				free(*arr);
				*arr = NULL;
            	arr++;
				bgrd_process(cmd, pid_bgrd_array, c_bgrd);
				break;
			case sml:
				free(*arr);
				*arr = NULL;
            	arr++;
				comand_if(cmd);
				break;
			default:
				comand_if(cmd);
				break;
		}    
    }
}

