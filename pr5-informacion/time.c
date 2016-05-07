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
