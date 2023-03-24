#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "client.h"
#include "interaction.h"
#include "buf.h"

#define DEFAULT_SERVER "lulu"
#define DEFAULT_PORT "7777"
#define SIZE_MESS 100


int main(int argc, char** argv) {
  char hostname[SIZE_MESS];
  char port[SIZE_MESS];
  if (argc < 3) {
    sprintf(hostname,"%s",DEFAULT_SERVER);
    sprintf(port,"%s",DEFAULT_PORT);
  }else{
    sprintf(hostname,"%s",argv[1]);
    sprintf(port,"%s",argv[2]);
  }
  struct sockaddr_in6* server_addr;
  int fdsock, adrlen;

  switch (get_server_addr(hostname,port, &fdsock, &server_addr, &adrlen)) {
    case 0: 
      printf("adresse creee !\n"); 
      break;
    case -1:
      fprintf(stderr, "Erreur: hote non trouve.\n"); 
      exit(1);
    case -2:
      fprintf(stderr, "Erreur: echec de creation de la socket.\n");
      exit(1);
  }

  int ret=1;
  //demander au client l'action voulue et l'exécuter
  if(choix_client(fdsock))
    ret = 0;

  close(fdsock);
  return ret;
}