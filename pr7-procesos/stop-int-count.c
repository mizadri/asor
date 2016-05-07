/*señales*/
#include <signal.h>
/*printf*/
#include <stdio.h>
#include <unistd.h>

volatile int i;

void handler(int signo){
	i = i + 1;
}


int main(){

	struct sigaction sig;
	i = 0;
	sig.sa_flags = SA_RESTART;
	sig.sa_handler = handler;

	sigaction(SIGINT, &sig, NULL); /*CTRL+C*/
	sigaction(SIGTSTP, &sig, NULL); /*CTRL+Z*/

	while(i < 10)
	{
		sleep(5);
		printf("Número de señales: %d\n", i);
		
	}
	return 0;
}
