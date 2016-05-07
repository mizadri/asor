#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
int main(){
	struct passwd *p;
	printf("Id real del proceso: %d\n",getuid());
	printf("Id efectivo del proceso: %d\n",geteuid());
	p = getpwuid(getuid());
	printf("Nombre de Login: %s\n", p->pw_name);
	printf("HOME: %s\n", p->pw_dir);
	printf("UID: %d\n", p->pw_uid);
	printf("Shell: %s\n", p->pw_shell);
	printf("Password: %s\n", p->pw_passwd);
	if (setuid(2) == -1){
		perror("Error:");
		return -1;
	}
	//Los permisos efectivos se modifican para un archivo si tiene activado el bit SUID
	//Lo que permite ejecutar el archivo como si tuvieras los permisos del propietario.
	//Este programa funcionaria con
	//>sudo chown root usu
	//>sudo chmod +s usu
	//Puede suponer un problema de seguridad
	printf("Id real del proceso: %d\n",getuid());
	printf("Id efectivo del proceso: %d\n",geteuid());
return 0;
}
