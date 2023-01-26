#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



void handler_power(int n){
    if (n==SIGTERM){
        printf("Fermeture de power_manager\n");
        exit(EXIT_SUCCESS);
    }
}

void main (int argc , char *argv[]) {

    struct sigaction sig_handler;
    sig_handler.sa_handler=handler_power;
    sigaction(SIGTERM,&sig_handler,NULL);
    sig_handler.sa_flags=SA_SIGINFO;

    FILE * fp;
 char c;
 while(1)
 {
	if(argc != 3) exit(EXIT_FAILURE);
 	fp = fopen (argv[1], "r");
	if(fp == NULL) exit(EXIT_FAILURE);
	c = fgetc(fp);
     printf("c=%c\n",c);
	fclose(fp);
	if(c =='1')
    {
        printf("[power manager] mise en veille en cours ...\n");
        kill(getppid(),SIGUSR1); //Envoie un signal au prog principal
	  fp = fopen (argv[1], "w"); 
	  fputs("0", fp);
   	  fclose(fp);
    }
    sleep(atoi(argv[2]));
 }
}
