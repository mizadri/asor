//-------------PRACTICAS DE REDES---------------------------------------
//1: maquina en laboratorio
1. asorregenerate
2. nano examen.topol

	netprefix inet
	//machine [id] [ifaz][redvirtual]
	machine 1 0 0
	machine 2 0 0 1 1
	machine 3 0 1
	// Esta configuracion de ejemplo tiene dos hosts(1 y 3) conectados via 
	// eth0 a dos redes diferentes, que se interconectan mediante un router(3)
 
3. vtopol examen.topol
//2: maquinas virtuales: user: root 
					    //pwd: cursoredes
1.Hosts:
	> ip addr add 192.168.0.1/24 dev eth0
	> ip addr add fd00:0:0:a::1/64 dev eth0
	> ip link set eth0 up // Esto nos otorga una fe80::net_id
	> ip route add default via 192.168.0.2
	> ip -6 route add default via fd00:0:0:0:a::3
	 
2.Router
	>[hosts excepto los default]
	>sysctl -w net.ipv6.conf.all.forwarding=1
	>sysctl net.ipv4.ip-forward=1
	
3.Configuracion dhcp
	1. Hosts
		> ip addr del 192.168.0.1/24 dev eth0
	2. Router
		1. Añadir seccion subnet '/etc/dhcp/dhcpd.conf'
		subnet 10.0.0.0 netmask 255.255.255.0 {
			range 10.0.0.11 10.0.0.50;
			option routers 10.0.0.3;
			option broadcast-address 10.0.0.255;
		}
		2. > service isc-dhcp-server start
	[3. Persistente:En host]
	Añadir a fichero: /etc/network/interfaces 
	//Con auto se hace al iniciar la maquina
	auto eth0
	iface eth0 inet dhcp
	
	>ifup eth0 [ifdown]	
	
3.Configuracion persistente ipv6 

	1. Añadir a fichero: /etc/network/interfaces
	
	auto eth0
	[iface eth0 inet dhcp]// Si quieres dhcp, necesitas server configurado
	iface eth0 inet6 static
		pre-up modprobe ipv6 #make sure the ipv6 module is loaded
		address 2607:f0d0:2001:a::10
		netmask 64
		gateway 2607:f0d0:2001:a::1
		//dns-nameservers 2001:DB8:0:76::101 2001:DB8:0:76::102
		
	auto eth0//quitar si lo has hecho ya antes de ipv6
	iface eth0 inet static
		address 192.168.1.42
		netmask 255.255.255.0
		gateway 192.168.1.1
		
	
	2. configurar/desconfigurar
		> ifdown eth0
		> ifup eth0 
		
		 
3.Anuncio de prefijos ipv6

	1. Eliminar configuracion en Hosts
		> ip addr del fd00:0:0:a::1/64 dev eth0
		> ip link set eth0 down
	2. En router 
		1. Activar (zebra=yes) en /etc/quagga/daemons
		2. Incluir configuracion en /etc/quagga/zebra.conf
		//Ej: /usr/share/doc/quagga/examples/zebra.conf.sample
		interface eth0
			no ipv6 nd suppress-ra
			ipv6 nd prefix fd00:0:0:a::/64
		interface eth1
			no ipv6 nd suppress-ra
			ipv6 nd prefix fd00:0:0:b::/64
		3. Arrancar demonio
			> service quagga restart
	3. Hosts
		> ip link set eth0 up
3.Comandos útiles

	> ping6 -I eth0 fe80::ff:fe00:100//si usamos fe80 necesitamos decir ifaz
	> ping6 fd00:0:0:b::4
	> ip addr
	> ip -6 address show eth0
	> route -e[-n para numeros]
	> ip route show [dev eth0]
	> ip route add 10.40.0.0/16 via 10.72.75.254
	> hping3 -p 23 -s --flood <ip_vict>[--rand-source]
	> netstat -at //(comprobar estado conexion)
	> watch netstat -t -o //(ver temporizador)
	
4.iptables

	> -A append | -I [n(=1 default)] -D n
	> iptables save|restore [file]
	> iptables ... -j REJECT --with-icmp-type
	
	> iptables -P FORWARD DROP //Politica por defecto mas segura
	> iptables -A FORWARD -p tcp --dport 80,22 -d <ip> -j ACCEPT
	> iptables -I FORWARD -m --state ESTABLISHED, RELATED -j ACCEPT
	> iptables -A INPUT -i eth0 -p tcp --tcp-flags ALL SYN -j DROP
											//me fijo | tienen que estar de esos	
												//ALL ALL (XMAS SCAN)
												//NULL NULL(NULL SCAN)
	> iptables -t nat -A POSTROUTING -o eth1 -j MASQUERADE
	 // nat dinamico
	> iptables -t nat -A PREROUTING -i eth1 -p tcp --dport 80 -j DNAT --to 192.168.0.1:7777
	 // esconder servidor web en otro puerto
	 
5.DNS 

	1. /etc/bind/named.conf
		//Comprobar sintaxis: > named-checkconf
		1. Comentar "include"
		2. Añadir dos zonas forward y reverse.
		zone "labfdi.es" {
			type master;
			file "/etc/bind/db.labfdi.es";
		}

		zone "0.168.192.in-addr.arpa" {
			type master;
			file "/etc/bind/db.0.168.192";
		}
		
	2.Crear archivos de zonas
	//Comprobar sintaxis: > named-checkconf <zona> <arcivo>
	forward:
			; db labfdi
			$TTL 28400

			;start of autority
			labfdi.es.  IN SOA ns.labfdi.es. hostmaster@labfdi.es. (
							2 ;serial
							172800 ; refresh
							900 ; update retry
							1209600 ; expiry
							3600); TTL

			labfdi.es.  IN NS ns.labfdi.es.

			www.labfdi.es.  IN A 192.168.0.200
					IN AAAA fd00::1
			ns.labfdi.es.  IN A 192.168.0.1
			mail.labfdi.es. IN A 192.168.0.250

			labfdi.es.      IN MX 10 mail.labfdi.es.

			servidor.labfdi.es. IN CNAME mail.labfdi.es.
			
	reverse:
			; db 0.168.192
			$TTL 604800

			;start of autority
			0.168.192.in-addr.arpa.  IN SOA ns.labfdi.es hostmaster@labfdi.es. (
							2 ;serial
							172800 ; refresh
							900 ; update retry
							1209600 ; expiry
							3600); TTL

			0.168.192.in-addr.arpa. IN NS ns.labfdi.es.
			1.0.168.192.in-addr.arpa. IN PTR ns.labfdi.es.
			200.0.168.192.in-addr.arpa. IN PTR www.labfdi.es.
			250.0.168.192.in-addr.arpa. IN PTR mail.labfdi.es.
			
	3. Activar demonio en servidor
		> service bind9 start
		> service bind9 status
	4. Configurar host cliente para usar el servidor al hacer consultas
		;;;Archivo '/etc/resolv.conf':

		search ns.labfdi.es.
		nameserver 192.168.0.1
	5. Reiniciar demonio del host cliente.
		> service bind9 restart
		
	COMANDOS DNS
	> host -t [registro: a mx ns] www.google.com.
	> host <ip> //devuelve pointer a hostname
	> host <hostname> //devueltve ip
	> dig @dnsserver hostname
	> dig -x ip
	> dig +trace hostname // consulta paso a paso
	> host -v <direccion> [servidor DNS]
	
	Recursividad forward:
	
	<host>
	1.  host -v www.fdi.ucm.es. a.root-servers.net.
	2.  host -v www.fdi.ucm.es. a.nic.es.
	3.  host -v www.fdi.ucm.es. sun.rediris.es.
	<dig>
	1. dig @a.root-servers.net. www.fdi.ucm.es
	2. dig @a.nic.es. www.fdi.ucm.es
	3. dig @chico.rediris.es. www.fdi.ucm.es

	;; ANSWER SECTION:
	www.fdi.ucm.es.		86400	IN	A	147.96.85.71

	
	Recursividad reverse:
	1. host -v 147.96.85.71 a.in-addr-servers.arpa.
	2. host -v 147.96.85.71 u.arin.net.
    3. host -v 147.96.85.71 chico.rediris.es.
	;; ANSWER SECTION:
	71.85.96.147.in-addr.arpa. 86400 IN	PTR	www.fdi.ucm.es.

	
	
				
7. Configuracion RIP. Todos los hosts: 
	1. > cd /etc/quagga
	2. > nano daemons //zebra=yes && (ripd=yes||ripng=yes)
	// > cp /usr/share/doc/quagga/examples/ripd.conf.sample ripd.conf||ripng.conf(=ipv6)
	3. > touch zebra.conf; nano ripd.conf
	router rip[ //|| ripng(=ipv6)]
	version 2 // || sin esta linea ipv6
	network eth0
	network eth1
	network eth2
	network eth3
	4. > service quagga restart 
	
	5.(opcional: conectarnos con telnet para configurar)
		1. ripd.conf ->Borrar todo y poner:
		no router rip
		password redes
		2. service quagga restart
		3. telnet localhost ripd // insertar password
		
//-------------PRACTICAS DE SISTEMAS OPERATIVOS-------------------------
//----------------------------------------------------------------------
//-----------5-INFORMACIÓN DEL SISTEMA DE FICHEROS----------------------
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

//---------------INFORMACION DE PROCESO---------------------------------
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

//--------------INFORMACION DE TIEMPO-----------------------------------
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
int main(){
	time_t theTime = time(NULL);

	printf("time(NULL): %d\n", theTime);
	printf("ctime(time): %s", ctime(&theTime));
	int i = 0;
	int a = 1;
	struct timeval tim;
	
	gettimeofday(&tim,NULL);
	double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
	for (i; i < 100000; ++i){
		++a;
	}
	gettimeofday(&tim, NULL);
	double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
	printf("%.6lf s transcurridos\n", t2-t1);

	struct tm *localT;
	localT = localtime(&theTime);
	printf("Estamos en el año %d\n", localT->tm_year+1900);
	char buffer[50];

	strftime(buffer, 50, "Hoy es %A,%H:%M", localT);
	printf("%s\n",buffer);
	return 0;
}

//----------------------------------------------------------------------
//----------------6-SISTEMA DE FICHEROS---------------------------------
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

// enlaces de ficheros
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
//----------------------------------------------------------------------
//----------------7-PROCESOS--------------------------------------------
/*planificador*/
#include <sched.h>
/*printf*/
#include <stdio.h>
/*getpriority*/
#include <sys/time.h>
#include <sys/resource.h>

int main(){

	int i = 0; /*pid proceso actual*/
	int politica, nice;
	struct sched_param p;
	
	/*Política del proceso*/
	politica = sched_getscheduler(i);
	/*Obtener la prioridad del proceso i a partir de p*/
	sched_getparam(i,&p);
	/*Nice del proceso*/
	nice = getpriority(PRIO_PROCESS, i);

	if(politica == SCHED_FIFO)
		printf("Política: SCHED_FIFO\n");
	else if(politica == SCHED_RR)
		printf("Política: SCHED_RR\n");
	else if(politica == SCHED_OTHER)
		printf("Política: SCHED_OTHER\n");
	printf("Prioridad del proceso: %d\n", p.sched_priority);
	printf("Nice del proceso: %d\n", nice);

	printf("Valor máximo de prioridad: %d\n", sched_get_priority_max(politica));
	printf("Valor mínimo de prioridad: %d\n", sched_get_priority_min(politica));


	return 0;
}

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

// System.c
#include <stdlib.h>

int main(int argc, char** argv){
	return system(argv[1]);
	//    ./sys "ls ­l"
}

//demonizar: dups, fork y exec
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

//señales con pending
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

/*Creo una variable en la terminal: SLEEP_SECS = 3
 Luego exporto la variable: export SLEEP_SECS
 Puedo compilar.*/

int main() {

	sigset_t mascara;
	sigemptyset(&mascara);
	sigaddset(&mascara, SIGINT);
	sigaddset(&mascara, SIGTSTP);

	if (sigprocmask(SIG_BLOCK, &mascara, NULL) == -1) {
		perror("No se puede borrar las señales");
		return -1;
	}
	char *sleep_secs = getenv("SLEEP_SECS");
	if (sleep_secs == NULL) {
		printf("No se puede leer SLEEP_SECS\n");
		return -1;
	}

	int secs = atoi(sleep_secs);
	if (secs < 0) secs = 0;
	
	sleep(secs);

	sigset_t pendiente;
	sigemptyset(&pendiente);
	if (sigpending(&pendiente) == -1) {
		perror("No hay señales pendientes");
		return -1;
	}
	switch (sigismember(&pendiente, SIGINT)) {
		case 1:
			printf("Se ha recibido SIGINT durante sleep\n");
			break;
		case -1:
			perror("No hay señal pendiente SIGINT");
			return -1;
			break;
	}
	switch (sigismember(&pendiente, SIGTSTP)) {
		case 1:
			printf("Se ha recibido SIGTSTP durante sleep, desbloqueando señal...\n");
			sigdelset(&mascara, SIGINT);
			if (sigprocmask(SIG_UNBLOCK, &mascara, NULL) == -1) {
				perror("No se puede desbloquear las señales");
				return -1;
			}
			break;
		case -1:
			perror("No hay señal pendiente SIGTSTP");
			return -1;
			break;
	}
	printf("Termino el programa\n");

	return 0;
}

/*señales con sigaction*/
#include <signal.h>
/*printf*/
#include <stdio.h>
#include <unistd.h>


void handler(int signo){

	if(signo == SIGINT) printf("Señal SIGINT\n");
	else if(signo == SIGTSTP) printf("Señal SIGTSTP\n");

}


int main(){


	sigset_t grupo;
	struct sigaction sig;
	sig.sa_flags = SA_RESTART;
	sig.sa_handler = handler;

	sigemptyset(&grupo);
	sigaddset(&grupo, SIGINT);
	sigaddset(&grupo, SIGTSTP);


	sigprocmask(SIG_BLOCK, &grupo, NULL);
	printf("Señales bloqueadas.\n");
	
	sigaction(SIGINT, &sig, NULL); /*CTRL+C*/
	sigaction(SIGTSTP, &sig, NULL); /*CTRL+Z*/
	
	sleep(10);


	sigprocmask(SIG_UNBLOCK, &grupo, NULL);
	printf("Señales desbloqueadas.\n");

	return 0;
}

/*contador de señales*/
#include <signal.h>
/*printf*/
#include <stdio.h>
#include <unistd.h>

volatile int i;

void handler(int signo){
	i = i + 1;
}


int main(){

	struct sigaction sig;
	i = 0;
	sig.sa_flags = SA_RESTART;
	sig.sa_handler = handler;

	sigaction(SIGINT, &sig, NULL); /*CTRL+C*/
	sigaction(SIGTSTP, &sig, NULL); /*CTRL+Z*/

	while(i < 10)
	{
		sleep(5);
		printf("Número de señales: %d\n", i);
		
	}
	return 0;
}

/*señales sigusr-bomba: borrar ejecutable*/
#include <signal.h>
/*printf*/
#include <stdio.h>
/*perror*/
#include <errno.h>
/*unlink, alarm*/
#include <unistd.h>
/**/
#include <string.h>
#include <stdlib.h>

volatile int desac, alarma;

void handler(int signo){

	if(signo == SIGUSR1) desac = 1; /*destruccion desactivada*/
	if(signo == SIGALRM) alarma = 1; /*alarma activada*/

}

int main(int argc, char **argv){


	sigset_t grupo;
	struct sigaction sig;
	desac= 0;

	if (argc!=2) perror("Error: número de argumentos.\n");
	sig.sa_flags = SA_RESTART;
	sig.sa_handler = handler;

	sigfillset(&grupo);
	sigdelset(&grupo, SIGUSR1);
	sigdelset(&grupo, SIGALRM);


	sigaction(SIGUSR1, &sig, NULL); /*ctr+u*/
	sigaction(SIGALRM, &sig, NULL); 
	
	alarm(atoi(argv[1]));
	sigsuspend(&grupo);
	if(desac)
	{	
		printf("Autodestrucción desactivada.\n"); 
		return -1;
	}
	if(alarma) unlink(argv[0]);

	return 0;
}
//----------------------------------------------------------------------
//----------------8-pipes-----------------------------------------------
//-----------: man -L en pipe | tail -n 60 > pipe.c --------------------
//pipes y exec
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

//pipe bidireccional
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define PW 1
#define PR 0

//Ejemplo DE PIPES man -L en pipe
int main(int argc, char** argv){
	int tuberia1[2], tuberia2[2], readb;
	pid_t pid;
	
	int num = 0;
	
	pipe(tuberia1);
	pipe(tuberia2);

	pid = fork();
	if(pid == -1) perror("Error de pid.\n");
	else if(pid == 0) /*hijo*/
	{	
		char buffer[256];
		char mensaje[1] = {'l'};
		close(tuberia1[PW]);
		close(tuberia2[PR]);
		int i;
		for(i = 0; i < 10; i++){
			readb = read(tuberia1[PR], buffer,256);
			buffer[readb] = '\0';
			printf("[Hijo], mensaje leído: %s\n", buffer);
			sleep(1);
			if(i == 9) mensaje[0] = 'q';
			write(tuberia2[PW], mensaje, 1);

		}
		close(tuberia2[PW]);
		close(tuberia1[PR]);
		exit(0);
	}
	else /*padre*/
	{
		close(tuberia1[PR]);
		close(tuberia2[PW]);
		char buffer_p[256];
		char mensaje_p[1] = {'l'};

		while(mensaje_p[0] != 'q'){
			printf("[Padre], nuevo mensaje: ");
			scanf("%s", buffer_p);
			//readb = read(0, buffer_p,256);
			//buffer_p[readb]= '\0';
			write(tuberia1[PW], buffer_p, strlen(buffer_p));
			while(mensaje_p[0] != 'l' && mensaje_p[0]!= 'q'){			
				read(tuberia2[PR], mensaje_p,1);
			}
			
		}
		close(tuberia1[PW]);
		close(tuberia2[PR]);
	}
	waitpid(pid, NULL, 0);
	
	return 0;
}

//----------------------------------------------------------------------
//----------------9-Sockets---------------------------------------------
//----: man -L en getaddrinfo | tail -n 200 > sockets.c ----------------
//----------------------------------------------------------------------
//tcp server con forks y evitar zombies
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 50

int main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;
	struct sockaddr_storage peer_addr;
	socklen_t peer_addr_len;
	ssize_t nread;
	char buf[BUF_SIZE];
	int nhijos = 0;

	if (argc != 2) {
	fprintf(stderr, "Usage: %s port\n", argv[0]);
	exit(EXIT_FAILURE);
	}

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET6;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* tcp socket; udp: SOCK_DGRAM */
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
	hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	s = getaddrinfo(NULL, argv[1], &hints, &result);
	if (s != 0) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
	exit(EXIT_FAILURE);
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
	sfd = socket(rp->ai_family, rp->ai_socktype,
	       rp->ai_protocol);
	if (sfd == -1)
	   continue;
	if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
	   break;                  /* Success */

	close(sfd);
	}

	if (rp == NULL) {               /* No address succeeded */
	fprintf(stderr, "Could not bind\n");
	exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);           /* No longer needed */

	/* Read datagrams and echo them back to sender */
	if(listen(sfd, 5)==-1){
			perror("Fallo de listen");
			exit(EXIT_FAILURE);
	}
	int cfd;
		for (;;) {
		
		peer_addr_len = sizeof(struct sockaddr_storage);
		cfd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_len);
		
		if(cfd == -1){
			perror("Fallo al accept");
			exit(-1);
		}
	
		pid_t pid;
		pid = fork();
		
		if(pid == 0){
			
			char host[NI_MAXHOST], service[NI_MAXSERV];

			s = getnameinfo((struct sockaddr *) &peer_addr,
					   peer_addr_len, host, NI_MAXHOST,
					   service, NI_MAXSERV, NI_NUMERICSERV);
			if (s == 0)
			   printf("Conectado a %s:%s\n", host, service);
			else
			   fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

			while(1){
					
				nread = recv(cfd, buf, BUF_SIZE, 0);
				if (nread == -1){
					perror("Fallo al receive");
					exit(EXIT_FAILURE);
				}
				
				buf[nread] = '\0';
			
				printf("pid(%i): %s",getpid(),buf);//Viene \n en al leer el echo en el cliente
				
				if (send(cfd, buf, strlen(buf), 0) == -1){
				   fprintf(stderr, "Error sending response\n");
				   exit(EXIT_FAILURE);
				}
				if(strcmp("q\n", buf)==0){
					exit(EXIT_SUCCESS);
				}
		
			}
		}else if(pid == -1){
			perror("Fallo en el fork");
			exit(EXIT_FAILURE);
		}else{
				nhijos++;
				close(cfd);
		}
	}
	int i = 0;
	int status;
	while(i<=nhijos){
			wait(&status);
			i++;
	}
	exit(EXIT_SUCCESS);
}

//tcp client

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 50

int
main(int argc, char *argv[])
{
   struct addrinfo hints;
   struct addrinfo *result, *rp;
   int sfd, s, j;
   size_t len;
   ssize_t nread;
   char buf[BUF_SIZE];

   if (argc < 3) {
	   fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
	   exit(EXIT_FAILURE);
   }

   /* Obtain address(es) matching host/port */

   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
   hints.ai_socktype = SOCK_STREAM; /* tcp socket;udp: SOCK_DGRAM */
   hints.ai_flags = 0;
   hints.ai_protocol = 0;          /* Any protocol */

   s = getaddrinfo(argv[1], argv[2], &hints, &result);
   if (s != 0) {
	   fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
	   exit(EXIT_FAILURE);
   }

   /* getaddrinfo() returns a list of address structures.
	  Try each address until we successfully connect(2).
	  If socket(2) (or connect(2)) fails, we (close the socket
	  and) try the next address. */

   for (rp = result; rp != NULL; rp = rp->ai_next) {
	   sfd = socket(rp->ai_family, rp->ai_socktype,
					rp->ai_protocol);
	   if (sfd == -1)
		   continue;

	   if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;                  /* Success */

	   close(sfd);
   }

   if (rp == NULL) {               /* No address succeeded */
	   fprintf(stderr, "Could not connect\n");
	   exit(EXIT_FAILURE);
   }

   freeaddrinfo(result);           /* No longer needed */

   /* Send remaining command-line arguments as separate
	  datagrams, and read responses from server */
	
	while(1){
	  s = read(0, buf, BUF_SIZE);
	  if(s == -1){
			perror("Falla el read de stdin");
	  }
	  
	  buf[s] = '\0';
	  
	   if (write(sfd, buf, strlen(buf)) == -1) {
		   fprintf(stderr, "failed write\n");
		   exit(EXIT_FAILURE);
	   }

	   nread = read(sfd, buf, BUF_SIZE);
	   if (nread == -1) {
		   perror("Fallo al leer respuesta del servidor");
		   exit(EXIT_FAILURE);
	   }
	
	   buf[nread] = '\0';
		if(strcmp("q\n", buf)==0)
			exit(EXIT_SUCCESS);

	}
   
}

//Servidor UDP + select (recibe ordenes de stdin) 
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>



#define BUF_SIZE 500
#define MAX_OUTPUT_SIZE 50

int main(int argc, char *argv[])
{
   struct addrinfo hints;
   struct addrinfo *result, *rp;
   int sfd, s;
   struct sockaddr_storage peer_addr;
   socklen_t peer_addr_len;
   ssize_t nread;
   char buf[2];  
   struct timeval tv;

   if (argc != 2) {
	   fprintf(stderr, "Usage: %s port\n", argv[0]);
	   exit(EXIT_FAILURE);
   }

   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
   hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
   hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
   hints.ai_protocol = 0;          /* Any protocol */
   hints.ai_canonname = NULL;
   hints.ai_addr = NULL;
   hints.ai_next = NULL;

   s = getaddrinfo(NULL, argv[1], &hints, &result);
   if (s != 0) {
	   fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
	   exit(EXIT_FAILURE);
   }

   /* getaddrinfo() returns a list of address structures.
	  Try each address until we successfully bind(2).
	  If socket(2) (or bind(2)) fails, we (close the socket
	  and) try the next address. */

   for (rp = result; rp != NULL; rp = rp->ai_next) {
	   sfd = socket(rp->ai_family, rp->ai_socktype,
			   rp->ai_protocol);
			   
	   if (sfd == -1)
		   continue;

	   if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
		   break;                  /* Success */

	   close(sfd);
   }

   if (rp == NULL) {               /* No address succeeded */
	   fprintf(stderr, "Could not bind\n");
	   exit(EXIT_FAILURE);
   }

   freeaddrinfo(result);           /* No longer needed */

   /* Read datagrams and echo them back to sender */
	
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	
	while( buf[0] != 'q'){

		fd_set fset;
		FD_ZERO(&fset);
		FD_SET(0,&fset);
		FD_SET(sfd, &fset);
		
		s = select(sfd+1, &fset, NULL, NULL, &tv);
		if(s > 0){
			
			if(FD_ISSET(sfd, &fset)){
				peer_addr_len = sizeof(struct sockaddr_storage);
				nread = recvfrom(sfd, buf, sizeof(buf), 0,(struct sockaddr *) &peer_addr, &peer_addr_len);
				if (nread == -1){
					perror("Fallo al receive");
					exit(EXIT_FAILURE);  
				}
				 char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
				 if (getnameinfo((struct sockaddr *)&peer_addr, peer_addr_len, hbuf, sizeof(hbuf), sbuf, 
									sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0)
					printf("host=%s, serv=%s\n", hbuf, sbuf);
				
			}else if(FD_ISSET(0,&fset)){
					s = read(0, buf, 2);
					if(s < 0){
						perror("Fallo al leer comando de stdin");
					}	
			}
			
			if(buf[0]=='t' || buf[0]=='d'){
					time_t t;
					char now[MAX_OUTPUT_SIZE];
					time(&t);
					struct tm* timedata = localtime(&t);
					
					if(buf[0]=='t'){
						printf("Solicitud de hora\n");
						strftime(now, MAX_OUTPUT_SIZE, "%H:%M", timedata);
					}else{
						printf("Solicitud de fecha\n");
						strftime(now, MAX_OUTPUT_SIZE, "%d de %B del %Y", timedata);
					}
					if(FD_ISSET(sfd, &fset)){
						if(sendto(sfd, now, strlen(now), 0, (struct sockaddr *) &peer_addr, peer_addr_len)==-1){
								perror("Fallo al send");
								exit(EXIT_FAILURE);
						}
					}else{
						printf("%s\n",now);
					}
				}else if(buf[0] == 'q'){
					printf("Conexion cerrada.\n");
					
					
				}else{
					printf("Comando desconocido\n");
				}
			
		}
	}
   exit(EXIT_SUCCESS);
}
//----------------------------------------------------------------------
//----: man -L en getaddrinfo | tail -n 200 > sockets.c ----------------
//Ojo:
//-Ejemplo UDP. Cambiar hints.socktype a SOCK_STREAM para tcp-----------
//-hints.ai_family = AF_INET6 //evitar que se utilizen siempre ipv4-----

//-----------: man -L en pipe | tail -n 60 > pipe.c --------------------
