/*señales*/
#include <signal.h>
/*printf*/
#include <stdio.h>
#include <unistd.h>


void handler(int signo){

	if(signo == SIGINT) printf("Señal SIGINT\n");
	else if(signo == SIGTSTP) printf("Señal SIGTSTP\n");

}


int main(){


	sigset_t grupo;
	struct sigaction sig;
	sig.sa_flags = SA_RESTART;
	sig.sa_handler = handler;

	sigemptyset(&grupo);
	sigaddset(&grupo, SIGINT);
	sigaddset(&grupo, SIGTSTP);


	sigprocmask(SIG_BLOCK, &grupo, NULL);
	printf("Señales bloqueadas.\n");
	
	sigaction(SIGINT, &sig, NULL); /*CTRL+C*/
	sigaction(SIGTSTP, &sig, NULL); /*CTRL+Z*/
	
	sleep(10);


	sigprocmask(SIG_UNBLOCK, &grupo, NULL);
	printf("Señales desbloqueadas.\n");

	return 0;
}
