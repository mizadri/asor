/*stat, open*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
/*printf*/
#include <stdio.h>
/*error*/
#include <errno.h>
/*ctime*/
#include <time.h>

/*EMULADOR DE STAT*/
int main(){
	
	int fich;
	struct stat buf;

	fich = open("./ejercicio5", O_CREAT);
	if(fich == -1)
	{return -EACCES;}

	fstat(fich, &buf);
	/*Mostrar el major asociado al dispositivo*/
	printf("Número de major: %d\n", major(buf.st_rdev));
	/*Mostrar el minor asociado al dispositivo*/
	printf("Número de minor: %d\n", minor(buf.st_rdev));
	/*Mostrar el i-nodo del archivo*/
	printf("Número de i-nodo: %ld\n", buf.st_ino);
	/*Mostrar el tipo de archivo(directorio, enlace simbolico o archivo ordinario)*/
	if(S_ISREG(buf.st_mode)){
		printf("Archivo ordinario.\n");
	}
	else if(S_ISDIR(buf.st_mode))
	{
		printf("Directorio.\n");
	}
	else if(S_ISLNK(buf.st_mode))
	{
		printf("Enlace simbólico.\n");
	}
	/*La hora en la que se accedió el fichero por última vez*/
	printf("Hora de último acceso al fichero: %s\n", ctime(&buf.st_atime));
	return 0;
}
