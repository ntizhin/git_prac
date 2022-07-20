#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

//	char ***process - array vectors of processes and their arguments
//  int n - count of process (size array "process"s);

int pipeline(char ***process, int n){	
	// implementation pipeline
	int i;
	pid_t pid;
	int in = dup(0);
	int fd[2];
	int f0 = in;
	for(i = 0; i < n - 1; i++){
		pipe(fd);
		if((pid = fork()) == 0){ // child
			close(fd[0]);
			dup2(f0, 0);
			close(f0);
			dup2(fd[1], 1);
			close(fd[1]);
			execvp(process[i][0], process[i]);
			fprintf(stderr, "error: exec didn't work; pr = %s\n", process[i][0]);
			exit(2);
		} 
		else if(pid == -1){
			perror("Error: fork didn't work\n");
			exit(1);
		}
		else{ // parrent
			close(f0);
			f0 = fd[0];
			close(fd[1]);
		}
	}
	// n - process
	if((pid = fork()) == 0){ // child
		dup2(f0, 0);
		close(f0);
		execvp(process[i][0], process[i]);
		fprintf(stderr, "error: exec didn't work; pr = %s\n", process[i][0]);
		exit(2);
	} 
	else if(pid == -1){
		perror("Error: fork didn't work\n");
		exit(1);
	}
	else{ // parrent
		close(f0);
		close(0);	
		close(1);	
		int status;
		waitpid(pid, &status, 0);
		if(!WIFEXITED(status)) fprintf(stderr, "error: status = %x\n", status);
		else if(WEXITSTATUS(status)) fprintf(stderr, "exit_child = %x\n", status);
		return status;
	}
}


// getting i/o redirection files
char **pr_io(char **arr, int *n, int fd[2]){
		if(!( (strcmp(arr[0], ">")) && (strcmp(arr[0], "<")) && (strcmp(arr[0], ">>")) )){
			if(*n < 3) {
				perror("error: syntax\n");
				*n = 0;
				return NULL;
			}
			else{
				int flag = 0;
				switch(arr[0][0]){
					case '>':
						if(arr[0][1] != '>') flag = O_WRONLY | O_CREAT | O_TRUNC;
						else flag = O_WRONLY | O_APPEND | O_CREAT;
						break;
					default:
						flag = O_RDONLY;
						break;
				}
				int f;
				if((f = open(arr[1], flag, 0664)) == -1){
					fprintf(stderr,"error: Can't open file: %s\n", arr[1]);
					return NULL;
				}
				else{
					if(!strcmp(arr[0], "<")) fd[0] = f;
					else fd[1] = f;
					*n -= 2;
					return pr_io(arr + 2, n, fd);
				}
			}
		}
		else return arr;
}
