#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void handler(int signal){
	printf("Ejecutando tarea programada\n");
	
}

int main(int argc, char *argv[]){
	pid_t cpid;
	int secs;
	sigset_t grupo;
	struct sigaction sig;
	int fdOut;
	
	if(argc < 3){
	   fprintf(stderr, "Usage: %s s comando", argv[0]);
	   exit(EXIT_FAILURE);
	}

	secs = atoi(argv[1]);
	
	cpid = fork();
	if (cpid == -1) {
	   perror("fork");
	   exit(EXIT_FAILURE);
	}

	if (cpid == 0) {            /* Code executed by child */
		
		sig.sa_flags = SA_RESTART;
		sig.sa_handler = handler;
		
		sigfillset(&grupo);
		sigdelset(&grupo, SIGALRM);
		
		sigaction(SIGALRM, &sig, NULL);

		
		alarm(secs);
		sigsuspend(&grupo);
		printf("%s\n", argv[2]);
		fdOut = open("/tmp/examen.out" , O_RDWR | O_CREAT);
		dup2(fdOut, STDOUT_FILENO);
		
		execvp(argv[2], argv + 2);
		close(fdOut);

	} else {                    /* Padre termina */
		printf("Ejecución de tarea programada en %i segundos por proceso %i\n",secs, cpid);
	   exit(EXIT_SUCCESS);
	}
	return 0;
}
