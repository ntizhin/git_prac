#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int n = 0;
int flag = 1;

void SigHndlr(int s){
	if(s == SIGINT){
		n += 1;
		printf("Signal: n = %d\n", n);
		sleep(1);
		signal(SIGINT, SigHndlr);
	}
	if(s == SIGQUIT) flag--;
};

int main(){
	signal(SIGINT, SigHndlr);
	signal(SIGQUIT, SigHndlr);
	for(;flag;) printf("1\n");
	return 0;
}
