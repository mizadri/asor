#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


//Prototipos
void ejercicio2(void);

int main(int argc, char** argv){
	int fdOut, fdErr, fdIn;
	
	pid_t pid = fork();

	if(pid == -1) perror("Error fork");
	else if(pid == 0){
		//$>cd /tmp; touch daemon.err daemon.out null
		fdOut = open("/tmp/daemon.out", O_RDWR | O_CREAT);
		fdErr = open("/tmp/daemon.err",O_RDWR | O_CREAT);
		fdIn = open("/tmp/null",O_RDWR | O_CREAT);

		dup2(fdIn,STDIN_FILENO);
		dup2(fdOut,STDOUT_FILENO);
		dup2(fdErr,STDERR_FILENO);

		execv (argv[1],argv + 1);
				//o argv
		close(fdErr);
		close(fdOut);
		close(fdIn);
	}
	else{
		int status;
		wait(&status);
		printf("El comando termino de ejecutarse\n");
	}
	return 0;
}

