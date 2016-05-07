#include <stdio.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <errno.h>
int main(){
	printf("-uname-\n");
	struct utsname buf;
	if (uname(&buf) == -1){
		perror("Error uname:");
		printf("Numero del error %i\n", errno);
	}
	printf("Nombre del sistema: %s\n", buf.sysname);
	printf("Nombre del maquina: %s\n", buf.nodename);
	printf("Nombre de la actualizacion: %s\n", buf.release);printf("Nombre del version: %s\n", buf.version);
	printf("Nombre de la arquitectura: %s\n", buf.machine);

	printf("-sysconf-\n");
	printf("Longitud maxima de los argumentos: %ld\n", sysconf(_SC_ARG_MAX));
	printf("Numero maximo de hijos: %ld\n",sysconf(_SC_CHILD_MAX));
	printf("Numero maximo de ficheros: %ld\n",sysconf(_SC_OPEN_MAX));

	printf("-pathconf-\n");
	printf("Numero maximo de enlaces: %ld\n", pathconf("/",
	_PC_LINK_MAX));
	printf("Tamaño maximo de la ruta: %ld\n", pathconf("/",
	_PC_PATH_MAX));
	printf("Tamaño maximo de un nombre de fichero: %ld\n",
	pathconf("/", _PC_NAME_MAX));
return 0;
}
