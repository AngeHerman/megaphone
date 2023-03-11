#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "client.h"

void test(){
    printf("TEST\n");
}

void affiche_adresse(struct sockaddr_in6 *adr){
    char adr_buf[INET6_ADDRSTRLEN];
    memset(adr_buf, 0, sizeof(adr_buf));
    
    inet_ntop(AF_INET6, &(adr->sin6_addr), adr_buf, sizeof(adr_buf));
    printf("adresse serveur : IP: %s port: %d\n", adr_buf, ntohs(adr->sin6_port));
}

int get_server_addr(char* hostname, char* port, int * sock, struct sockaddr_in6** addr, int* addrlen) {
  struct addrinfo hints, *r, *p;
  int ret;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_V4MAPPED | AI_ALL;

  if ((ret = getaddrinfo(hostname, port, &hints, &r)) != 0 || NULL == r){
    fprintf(stderr, "erreur getaddrinfo : %s\n", gai_strerror(ret));
    return -1;
  }
  
  *addrlen = sizeof(struct sockaddr_in6);
  p = r;
  while( p != NULL ){
    affiche_adresse((struct sockaddr_in6 *) p->ai_addr);
    if((*sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) > 0){
      if(connect(*sock, p->ai_addr, *addrlen) == 0)
	  break;
      
      close(*sock);
    }

    p = p->ai_next;
  }

  if (NULL == p) return -2;

  //on stocke l'adresse de connexion
  *addr = (struct sockaddr_in6 *) p->ai_addr;

  //on libère la mémoire allouée par getaddrinfo 
  freeaddrinfo(r);
    
  return 0;
}
