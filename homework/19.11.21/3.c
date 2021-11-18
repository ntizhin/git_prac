#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int n_int = 0;
int n_quit = 0;
int flag = 1;

void SigHndlr(int s){
	if(s == SIGINT){
		n_int++;
		printf("Signal: n = %d\n", n_int);
		sleep(1);
		if(n_int == 5) flag--;
		else signal(SIGINT, SigHndlr);
	}
	if((s == SIGQUIT) && (n_int > 1) && (n_int < 4)) n_quit++;
};

int main(){
	signal(SIGINT, SigHndlr);
	signal(SIGQUIT, SigHndlr);
	for(;flag;) printf("1\n");
	printf("Signal trap: n = %d\n", n_quit);
	return 0;
}
