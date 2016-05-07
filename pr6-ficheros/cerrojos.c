#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
	if(argc != 2) {
		printf("Usage: ./ej2 <fname>\n");
		return -1;
	}
	int fd;
	fd = open(argv[1], O_CREAT | O_RDWR, 0777);
	if (fd == -1) {
		printf("Error opening file, %i - %s\n", errno, strerror(errno));
		return -1;
	}
	struct flock fl;
	
	fl.l_type = F_WRLCK; /* F_RDLCK, F_WRLCK, F_UNLCK*/
	fl.l_whence = SEEK_SET; /* SEEK_SET(Start), SEEK_CUR(puntero current), SEEK_END*/
	fl.l_start = 0;	/* Offset from l_whence*/
	fl.l_len= 0;/* length, 0 = to EOF*/
	fl.l_pid= getpid(); /* our PID*/
	

	/* Primero comprobar con GETLK si hay un candado
	, si no hay ni WR ni RD podemos obtenerlo nostros */
	if (fcntl(fd, F_GETLK, &fl) == -1) {
		/* Handle unexpected error */;
		printf("Error getting lock, %i - %s\n", errno, strerror(errno));
		exit(-1);
	}
	if(fl.l_type == F_WRLCK){
		printf("Process %ld has a write lock already!\n", fl.l_pid);
		printf("%i - %s\n", errno, strerror(errno));
		exit(-1);
	} else if (fl.l_type == F_RDLCK) {
		printf("Process %ld has a read lock already!\n", fl.l_pid);
		printf("%i - %s\n", errno, strerror(errno));
		exit(-1);
	}
	if (fcntl(fd, F_SETLK, &fl) == -1) {
		
		perror("Fallo al obtener el candado");
		exit(-1);
		
	} else { /* Lock was granted... */
		/* Perform I/O on bytes 0 to EOF of file */
		time_t tim;
		tim = time(NULL);
		struct tm * lt;
		lt = localtime(&tim);
		int MAX = 100;
		const char * buf[MAX],* format = "Tiempo %T";
		int length = strftime(buf, MAX, format, lt);
		if(length == 0){
			puts("Couldn't format time");
			exit(EXIT_FAILURE);
		}
		write(fd, buf, length);
		/* Unlock the locked bytes */
		fl.l_type = F_UNLCK;
		fl.l_whence = SEEK_SET;
		fl.l_start = 0;
		fl.l_len = 0;
		if (fcntl(fd, F_SETLK, &fl) == -1){
			/* Handle error */;
			printf("Error removing lock, %i - %s\n", errno, strerror(errno));
			exit(-1);
		}
	}
	exit(EXIT_SUCCESS);
}
