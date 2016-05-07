#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define PW 1
#define PR 0

//Ejemplo DE PIPES man -L en pipe
int main(int argc, char** argv){
	int tuberia1[2], tuberia2[2], readb;
	pid_t pid;
	
	int num = 0;
	
	pipe(tuberia1);
	pipe(tuberia2);

	pid = fork();
	if(pid == -1) perror("Error de pid.\n");
	else if(pid == 0) /*hijo*/
	{	
		char buffer[256];
		char mensaje[1] = {'l'};
		close(tuberia1[PW]);
		close(tuberia2[PR]);
		int i;
		for(i = 0; i < 10; i++){
			readb = read(tuberia1[PR], buffer,256);
			buffer[readb] = '\0';
			printf("[Hijo], mensaje leído: %s\n", buffer);
			sleep(1);
			if(i == 9) mensaje[0] = 'q';
			write(tuberia2[PW], mensaje, 1);

		}
		close(tuberia2[PW]);
		close(tuberia1[PR]);
		exit(0);
	}
	else /*padre*/
	{
		close(tuberia1[PR]);
		close(tuberia2[PW]);
		char buffer_p[256];
		char mensaje_p[1] = {'l'};

		while(mensaje_p[0] != 'q'){
			printf("[Padre], nuevo mensaje: ");
			scanf("%s", buffer_p);
			//readb = read(0, buffer_p,256);
			//buffer_p[readb]= '\0';
			write(tuberia1[PW], buffer_p, strlen(buffer_p));
			while(mensaje_p[0] != 'l' && mensaje_p[0]!= 'q'){			
				read(tuberia2[PR], mensaje_p,1);
			}
			
		}
		close(tuberia1[PW]);
		close(tuberia2[PR]);
	}
	waitpid(pid, NULL, 0);
	
	return 0;
}



