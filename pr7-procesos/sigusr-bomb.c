/*señales*/
#include <signal.h>
/*printf*/
#include <stdio.h>
/*perror*/
#include <errno.h>
/*unlink, alarm*/
#include <unistd.h>
/**/
#include <string.h>
#include <stdlib.h>

volatile int desac, alarma;

void handler(int signo){

	if(signo == SIGUSR1) desac = 1; /*destruccion desactivada*/
	if(signo == SIGALRM) alarma = 1; /*alarma activada*/

}

int main(int argc, char **argv){


	sigset_t grupo;
	struct sigaction sig;
	desac= 0;

	if (argc!=2) perror("Error: número de argumentos.\n");
	sig.sa_flags = SA_RESTART;
	sig.sa_handler = handler;

	sigfillset(&grupo);
	sigdelset(&grupo, SIGUSR1);
	sigdelset(&grupo, SIGALRM);


	sigaction(SIGUSR1, &sig, NULL); /*ctr+u*/
	sigaction(SIGALRM, &sig, NULL); 
	
	alarm(atoi(argv[1]));
	sigsuspend(&grupo);
	if(desac)
	{	
		printf("Autodestrucción desactivada.\n"); 
		return -1;
	}
	if(alarma) unlink(argv[0]);

	return 0;
}
