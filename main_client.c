#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "client.h"
#include "buf.h"

#define DEFAULT_SERVER "lulu"
#define DEFAULT_PORT "7777"
#define SIZE_MESS 100

int main(int argc, char** argv) {
  uint16_t id_client = -1;

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
    case 0: printf("adresse creee !\n"); break;
    case -1:
      fprintf(stderr, "Erreur: hote non trouve.\n"); 
    case -2:
      fprintf(stderr, "Erreur: echec de creation de la socket.\n");
      exit(1);
  }
  affiche_adresse(server_addr);
  id_client = demande_inscription(fdsock, "aaaaaa");
  printf("id client est %d\n",id_client);
  close(fdsock);
  return 0;
}