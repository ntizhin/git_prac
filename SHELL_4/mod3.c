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
		wait(&status);
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

void bgrd_process(char **arr){
	int f, status;
	int pid = fork();
	switch (pid)
	{
	case -1:
		perror("Error: fork didn't work\n");
		break;
	case 0:
		if((f = open("./bgrd_file.txt", O_RDONLY | O_WRONLY | O_TRUNC | O_CREAT)) == -1){
			perror("Can't open file for bgrd_process\n");
			exit(1);
		}
		else{
			int pid_bgrd = fork();
			switch (pid_bgrd)
			{
			case -1:
				perror("Error: fork didn't work\n");
				exit(1);
				break;
			case 0:
				dup2(f, 0);
				dup2(f, 1);
				close(f);
				setpgrp();
				comand_if(arr);
				exit(0);
				break;
			default:
				close(f);
				printf("pid_background_process = %d\n", pid_bgrd);
				exit(0);
				break;
			}
		}
	default:
		wait(&status);
		if(status){
			perror("error: child from bgrd_process\n");
			printf("status = %x\n", status);
		}
		break;
	}
}

void comand_shell(char **arr){
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
				bgrd_process(cmd);
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

