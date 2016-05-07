//sesion e info-proceso
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>


void ejercicio2(void){
	pid_t pid, ppid, pgid, sid;
	struct rlimit limit;
	char dir [100];

	pid  = getpid();
	ppid = getppid();
	pgid = getpgid(pid);
	sid  = getsid(pid);

	printf("pid  = %d\n",pid);
	printf("ppid = %d\n",ppid);
	printf("pgid = %d\n",pgid);
	printf("sid  = %d\n",sid);
	printf("RLIMIT_FSIZE = %d\n", getrlimit(RLIMIT_FSIZE, &limit));
	printf("path = %s\n", getcwd(dir, 100));
}


void ejercicio3(void){
	pid_t forked;

	printf("EJER3--------------\n");
	forked = fork();


	if (forked == -1){//Hijo	
		perror("error fork");
		//exit(-1);
	}
	else if (forked == 0){
		printf("Nueva sesion = %d\n", setsid());
		chdir("/tmp");
		ejercicio2();
	}
	else{//Padre
		int signal;
		wait(&signal);
	}
}

int main(int argc, char** argv){

	ejercicio2();
	ejercicio3();

	return 0;
}
