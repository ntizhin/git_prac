#ifndef MOD3_H_SENTRY
#define MOD3_H_SENTRY

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

void pipeline(char ***process, int n);
char **pr_io(char **arr, int *n, int fd[2]);

#endif
