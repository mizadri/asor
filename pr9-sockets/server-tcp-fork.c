#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 50

int
main(int argc, char *argv[])
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
	hints.ai_socktype = SOCK_STREAM; /* tcp socket */
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
