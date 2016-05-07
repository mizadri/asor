/*link, symlink*/
#include <unistd.h>
/*printf*/
#include <stdio.h>

int main(){

	link("./ejercicio5","./enlaceDuro.hard");
	printf("Creado enlace duro.\n");
	symlink("./ejercicio5", "./enlaceSym.sym");
	printf("Creado enlace simbólico.\n");
	return 0;

}
