#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

void handler_time(int n){
    if (n==SIGTERM){
        printf("Fermeture de get_time\n");
        exit(EXIT_SUCCESS);
    }
}

void main (void)
{
    struct sigaction sig_handler;
    sig_handler.sa_handler=handler_time;
    sigaction(SIGTERM,&sig_handler,NULL);

   time_t now = time (NULL);

   struct tm tm_now = *localtime (&now);

   char s_now[sizeof "JJ/MM/AAAA HH:MM:SS"];

   strftime (s_now, sizeof s_now, "%d/%m/%Y %H:%M:%S", &tm_now);

   printf ("[get time] %s\n", s_now);
   exit(EXIT_SUCCESS);
}
