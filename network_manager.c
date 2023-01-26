#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <signal.h>
#include <stdlib.h>

void handler_network(int n){
    if (n==SIGTERM){
        printf("Fermeture de network_manager\n");
        exit(EXIT_SUCCESS);
    }
}


//recupere l'adresse ip de chaque carte réseau chaque 5 secondes
int main() {

    struct sigaction sig_handler;
    sig_handler.sa_handler=handler_network;
    sigaction(SIGTERM,&sig_handler,NULL);

    struct ifaddrs *addrs, *tmp;
  while (1) {
     getifaddrs(&addrs);
     tmp = addrs;

     while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
          struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
          printf("[network manager] %s: %s\n", tmp->ifa_name, inet_ntoa(pAddr->sin_addr));
        }
        tmp = tmp->ifa_next;
     }

     freeifaddrs(addrs);

     sleep(5);
  }

  return 0;
}
