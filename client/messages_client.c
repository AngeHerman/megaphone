#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "messages_client.h"



uint16_t entete_message(uint16_t code_req, uint16_t id){
    uint16_t res = 0;

    res+=code_req;
    id = id<<5;
    res+=id;
    
    return htons(res);
}

char * message_inscription_client(char * pseudo){
    char * res = (char *)malloc(LEN_MESS_INSCR * sizeof(char));
    if(res==NULL){
        perror("malloc");
        return NULL;
    }
    uint16_t entete = entete_message(1,0);
    *((uint16_t *) res) = entete;
    
    unsigned int len_pseudo = strlen(pseudo);
    memmove(res+2, pseudo, len_pseudo);    
    /*compléter par des '#' si pseudo n'a pas 10 caractères*/
    for(int i=0; i<LEN_PSEUDO-len_pseudo; i++){
        res[2+len_pseudo+i] = '#';
    }

    return res;
}


char * message_client(uint16_t code_req, uint16_t id, uint16_t numfil, uint16_t nb, uint8_t datalen, char * data){
  //7 octets + le nombre d'octets pour le texte du billet
  char * res = (char *)malloc(7 + (datalen * sizeof(char)));
  if(res == NULL){
    perror("malloc");
    return NULL;
  }
  //remplir l'entête
  ((uint16_t *)res)[0] = entete_message(code_req,id);
  //les autres champs
  ((uint16_t *)res)[1] = htons(numfil);
  ((uint16_t *)res)[2] = htons(nb);
  ((u_int8_t *)res)[6] = datalen;

  //copier le texte du message
  if(datalen > 0){
    memmove(res+7, data, datalen);    
  }

  return res;
}




char * message_dernier_billets(uint16_t id, uint16_t numfil, uint16_t nb){
  uint16_t code_req = CODE_REQ_DEMANDE_BILLETS;
  uint8_t datalen = 0;
  return message_client(code_req,id,numfil,nb,datalen,"");
}

u_int16_t reponse_derniers_billets(u_int16_t * rep){
  uint8_t cod_req;
  uint16_t id;
  u_int16_t numfil = ntohs(rep[1]);
  u_int16_t nb = ntohs(rep[2]);
  u_int16_t entete = ntohs(rep[0]);
  u_int16_t masque = 0b0000000000011111;
  cod_req = entete & masque;
  id = (entete & ~masque) >> 5;
  

  if(cod_req!= CODE_REQ_DEMANDE_BILLETS){
    fprintf(stderr, "reponse de demande des derniers billets erroné et le code reçu est %d\n",cod_req);
    return -1;
  }
  
  printf("id est %u codereq est %u Numfil est %u et Nb est %u\n",id,cod_req,numfil,nb);
  return nb;
}