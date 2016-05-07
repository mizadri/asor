#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

/*Creo una variable en la terminal: SLEEP_SECS = 3
 Luego exporto la variable: export SLEEP_SECS
 Puedo compilar.*/

int main() {

	sigset_t mascara;
	sigemptyset(&mascara);
	sigaddset(&mascara, SIGINT);
	sigaddset(&mascara, SIGTSTP);

	if (sigprocmask(SIG_BLOCK, &mascara, NULL) == -1) {
		perror("No se puede borrar las señales");
		return -1;
	}
	char *sleep_secs = getenv("SLEEP_SECS");
	if (sleep_secs == NULL) {
		printf("No se puede leer SLEEP_SECS\n");
		return -1;
	}

	int secs = atoi(sleep_secs);
	if (secs < 0) secs = 0;
	
	sleep(secs);

	sigset_t pendiente;
	sigemptyset(&pendiente);
	if (sigpending(&pendiente) == -1) {
		perror("No hay señales pendientes");
		return -1;
	}
	switch (sigismember(&pendiente, SIGINT)) {
		case 1:
			printf("Se ha recibido SIGINT durante sleep\n");
			break;
		case -1:
			perror("No hay señal pendiente SIGINT");
			return -1;
			break;
	}
	switch (sigismember(&pendiente, SIGTSTP)) {
		case 1:
			printf("Se ha recibido SIGTSTP durante sleep, desbloqueando señal...\n");
			sigdelset(&mascara, SIGINT);
			if (sigprocmask(SIG_UNBLOCK, &mascara, NULL) == -1) {
				perror("No se puede desbloquear las señales");
				return -1;
			}
			break;
		case -1:
			perror("No hay señal pendiente SIGTSTP");
			return -1;
			break;
	}
	printf("Termino el programa\n");

	return 0;
}
