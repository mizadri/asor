/*dup y dup2*/
#include <unistd.h>
/*open*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/*printf*/
#include <stdio.h>


int main(int argc, char **argv){
	int fd;
	fd=open(argv[1], O_CREAT | O_RDWR, 0640);
	/*Se cambia la salida estándar al fichero abierto */
	dup2(fd,1);
	dup2(fd,2);
	
	close(fd);
	/*EScribimos en el fichero*/
	printf("HOLA MUNDO\n");
	perror("ERROR.");
	return 0;
}
