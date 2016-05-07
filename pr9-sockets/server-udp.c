 #include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>

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

	while( buf[0] != 'q'){
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
			
			if(sendto(sfd, now, strlen(now), 0, (struct sockaddr *) &peer_addr, peer_addr_len)==-1){
					perror("Fallo al send");
					exit(EXIT_FAILURE);
			}
			
		}else if(buf[0] == 'q'){
			printf("Conexion cerrada por el cliente\n");
			if(sendto(sfd, "Conexion cerrada", 17, 0, (struct sockaddr *) &peer_addr, peer_addr_len)==-1){
					perror("Fallo al send");
					exit(EXIT_FAILURE);
			}
			exit(EXIT_SUCCESS);
			
		}else{
			printf("Comando desconocido\n");
		}

	}
   exit(EXIT_SUCCESS);
}
