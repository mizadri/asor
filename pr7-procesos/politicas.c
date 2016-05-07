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
