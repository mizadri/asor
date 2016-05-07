#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/*ESQUEMA SERVIDOR TCP:
	- socket()
	- bind()
	- listen()
	- accept()
	- recv()
	- send()
	- close()
*/
#define MAX_SIZE 80
int main(int argc, char** argv){

	int sd, info;
	struct addrinfo hints;
	struct addrinfo *res;
	char buffer[MAX_SIZE];
	int sdC = 0;
	int r = 0;
	int estado = 0;

	/*struct addrinfo*/
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET6;/*IPV4 O IPV6*/
	hints.ai_socktype = SOCK_STREAM; /*TCP*/
	hints.ai_protocol = 0;

	if(argc < 2){ perror("Error de argumentos.\n");}

	/*Devuelve una lista de estructuras de direcciones*/
	getaddrinfo(NULL,argv[1], &hints, &res);

	printf("Creando socket.\n");
	/*crea un extremo de comunicación*/
	sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	/*Define la dirección en la que se escuchará. Enlaza un nombre a un conector.*/
	printf("Bind.\n");
	bind(sd, (struct sockaddr *) res->ai_addr, res->ai_addrlen);
	
	struct sockaddr_storage cliente;
	socklen_t cliente_len = sizeof(cliente);
	char host[NI_MAXHOST];
	char serv[NI_MAXSERV];
	printf("Listen.\n");		
	if(listen(sd,10)){ return -1; }
	printf("Accept.\n");
	sdC = accept(sd, (struct sockaddr *) &cliente, &cliente_len);

	
	while(strcmp(buffer, "q") != 0 || strcmp(buffer, "q\n") != 0){
		/*struct sockaddr_storage cliente;
		socklen_t cliente_len = sizeof(cliente);
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];
		if(r == 0){	
			printf("Listen.\n");		
			if(listen(sd,10)){ return -1; }
			printf("Accept.\n");
			sdC = accept(sd, (struct sockaddr *) &cliente, &cliente_len);
		}*/
		printf("Recibiendo.\n");
		r = recv(sdC, buffer, MAX_SIZE, 0);
		buffer[r]= '\0';
		printf("r: %d\n",r);
		int pid;
		pid = fork();
		if(pid == -1){ perror("Error de fork.\n");}
		else if(pid == 0){ /*Hijo*/

			if(strcmp(buffer, "q") == 0 || strcmp(buffer, "q\n") == 0)
			{
				send(sdC, "ADIÓS..\n", 9, 0);
				freeaddrinfo(res);
				close(sd);
				printf("Conexión terminada\n");
				_exit(0);
			}

			/*Obtener host y service*/
			getnameinfo((struct sockaddr *) &cliente, cliente_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
			printf("Host: %s, Puerto: %s\n", host, serv);
			printf("Mensaje (%i): %s\n", r, buffer);
			send(sdC, buffer, r, 0);
			sleep(3);
		}
		else{ /*Padre: espera a que termine el hijo*/
			wait(&estado);
			printf("Termino el hijo..\n");
			if(strcmp(buffer, "q") == 0 || strcmp(buffer, "q\n") == 0)
			{
				freeaddrinfo(res);
				close(sd);
				_exit(1);
			}
		}
	}

	freeaddrinfo(res);
	close(sdC);
	close(sd);

	return 0;
}
