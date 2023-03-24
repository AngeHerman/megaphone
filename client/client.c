#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>

#include "client.h"
#include "messages_client.h"
#include "buf.h"



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



int demande_dernier_billets(int sock,u_int16_t id_client,uint16_t numfil, uint16_t nb,buf_t* buffer){

  char * mess_dernier_billets = message_dernier_billets(id_client,numfil,nb);
  if(send(sock, mess_dernier_billets,LEN_MESS_DMD_BILLETS,0) != LEN_MESS_DMD_BILLETS){
    free(mess_dernier_billets);
    return -1;
  } 

  free(mess_dernier_billets);
 
  //réponse du serveur
  u_int16_t rep[3];
  int taille = 0;
  if((taille = recv(sock,rep, sizeof(rep) ,0)) != sizeof(rep)){
    printf("Taille est %d\n",taille);
    return -1;
  }

  uint16_t nbb = reponse_derniers_billets(rep);
  char msg[SIZE_BUF +1];
  int result;
  memset(msg, 0, sizeof(SIZE_BUF+1));
  for(int i = 0; i < nbb; i++){
    result = read_buf(sock,buffer,msg,NB_OCTECS_DERNIERS_MESSAGE_JUSQUA_DATALEN);
    uint8_t datalen = ((uint8_t *)msg)[NB_OCTECS_DERNIERS_MESSAGE_JUSQUA_DATALEN -1];
    printf("La taille de ce qui arrive est %u\n",datalen);
    result = read_buf(sock,buffer,msg,datalen);
    printf("Message %d est <%s>\n",i,msg+NB_OCTECS_DERNIERS_MESSAGE_JUSQUA_DATALEN);
    
  }
  return nbb;

}

int demande_dernier_billets_tous_les_fils(int sock,u_int16_t id_client,buf_t* buffer){
  uint16_t numfil = 1;
  uint16_t nb = 2; // Pour l'instant je teste avec la demande des 4 derniers billets de tous les fils
  int result = demande_dernier_billets(sock,id_client,numfil,nb,buffer);
  return result;

}

//fonctions pour inscrire un client
u_int16_t reponse_inscription(uint16_t * rep){
  uint8_t cod_req;
  uint16_t id;
  u_int16_t numfil = ntohs(rep[1]);
  u_int16_t nb = ntohs(rep[2]);
  u_int16_t entete = ntohs(rep[0]);
  u_int16_t masque = 0b0000000000011111;
  cod_req = entete & masque;
  id = (entete & ~masque) >> 5;

  if(cod_req==31)
    return 0;

  if(cod_req!=1 || numfil!=0 || nb!=0){
    fprintf(stderr, "message de confirmation d'inscription erroné\n");
    return 0;
  }
  
  return id;
}

uint16_t demande_inscription(int fd_sock, char * pseudo){
  char * mess_inscr = message_inscription_client(pseudo);
  
  if(send(fd_sock, mess_inscr,LEN_MESS_INSCR,0) != LEN_MESS_INSCR){
    return 0;
  } 
  free(mess_inscr);
 
  //réponse du serveur
  u_int16_t rep[3];
  int len = 0;
  if( (len = recv(fd_sock,rep, sizeof(rep) ,0)) != sizeof(rep)){
    printf("%d\n", len);
    return 0;
  }
  uint16_t id = reponse_inscription(rep);
  return id;
}

uint16_t reponse_poster_billet(uint16_t * rep){
  uint8_t cod_req;
  uint16_t id;
  u_int16_t numfil = ntohs(rep[1]);
  u_int16_t nb = ntohs(rep[2]);
  u_int16_t entete = ntohs(rep[0]);
  u_int16_t masque = 0b0000000000011111;
  cod_req = entete & masque;
  id = (entete & ~masque) >> 5;

  if(cod_req==31)
    return 0;

  if(cod_req!=2 || nb!=0){
    fprintf(stderr, "message de confirmation d'inscription erroné\n");
    return 0;
  }
  
  return numfil;
}

//fonctions pour poster un billet sur un fil
uint16_t poster_un_billet(int sock,uint16_t id, uint16_t num_fil, uint8_t datalen, char * data){
  char * mess = message_client(2,id,num_fil,0,datalen,data);

  if(send(sock,mess,7+datalen,0)!=7+datalen){
    return 0;
  }
  free(mess);

  //réponse du serveur
  u_int16_t rep[3];
  int len = 0;
  if( (len = recv(sock,rep, sizeof(rep) ,0)) != sizeof(rep)){
    printf("%d\n", len);
    return 0;
  }
  return reponse_poster_billet(rep);
}