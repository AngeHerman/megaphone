#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "messages_serveur.h"

uint16_t entete_message(uint16_t code_req, uint16_t id){
    uint16_t res = 0;

    res+=code_req;
    id = id<<5;
    res+=id;
    
    return htons(res);
}

char * message_server(uint16_t code_req, uint16_t id, uint16_t numfil, uint16_t nb){
    char * res = (char*)malloc(sizeof(char) * 6);
    if(res==NULL)
        return NULL;
    
    //remplir l'entête
    ((uint16_t *)res)[0] = entete_message(code_req,id);
    //les autres champs
    ((uint16_t *)res)[1] = htons(numfil);
    ((uint16_t *)res)[2] = htons(nb);
    //TODO : numfil et nb pour code_req 2&3
    return res;
}

char * message_billet(uint16_t numfil, char* origine, char* pseudo, uint8_t datalen, char* data){
    char * res = (char*)malloc(sizeof(char) * (23+datalen));
    if(res==NULL)
        return NULL;
    ((uint16_t *)res)[0] = htons(numfil);
    memmove(res+2, origine, LEN_PSEUDO);
    memmove(res+2+LEN_PSEUDO, pseudo, LEN_PSEUDO);
    memmove(res+2+(2*LEN_PSEUDO), &datalen , 1);
    memmove(res+23, data, datalen);
    return res;
}


uint8_t get_code_req(uint16_t entete){
    uint16_t masque = 0b0000000000011111;
    uint8_t cod_req = entete & masque;
    return cod_req;
}

uint16_t get_id_requete(uint16_t entete){
    uint16_t masque = 0b0000000000011111;
    uint16_t id = (entete & ~masque) >> 5;
    return id;
}