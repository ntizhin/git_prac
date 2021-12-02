#ifndef MOD3_H_SENTRY
#define MOD3_H_SENTRY

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mod1.h"
#include "mod2.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

int comand(char **arr, int n);
void comand_if(char **arr, int size_arr);
void comand_shell(char **arr);

#endif
