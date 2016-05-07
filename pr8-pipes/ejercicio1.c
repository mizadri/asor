#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PW 1
#define PR 0

int main(int argc, char** argv){
	int tuberia[2];
	pid_t pid;

	pipe(tuberia);
	
	pid = fork();
	if(pid == -1) perror("Error de pid.\n");
	else if(pid == 0) /*hijo*/
	{	
		close(tuberia[PW]);
		dup2(tuberia[PR], 0);
		close(tuberia[PR]);
		execlp(argv[3],argv[3],argv[4],NULL);
		//perror(argv[0]);
		printf("HIJO.\n");
	}
	else /*padre*/
	{
		close(tuberia[PR]);
		dup2(tuberia[PW], 1);
		close(tuberia[PW]);
		execlp(argv[1],argv[1],argv[2],NULL);
		//perror(argv[0]);
		printf("PADRE.\n");
	}
	waitpid(pid, NULL, 0);
	return 0;
}



