/* exec.c ­ Listar los procesos del usuario usando exec. */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main (){
	int salida = 0;/* Salida del comando */
	printf ("Ejemplo de exec.\n");
	// execlp("ls", "ls", "­al", NULL);
	execl ("/bin/ls", "ls", "­l", NULL);
	printf ("Salida del comando: %d\n", salida);
exit (salida);
}

/* System.c
#include <stdlib.h>

int main(int argc, char** argv){
	return system(argv[1]);
	//    ./sys "ls ­l"
}*/
