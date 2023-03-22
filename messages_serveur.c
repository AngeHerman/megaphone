#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

uint16_t entete_message(uint16_t code_req, uint16_t id){
    uint16_t res = 0;

    res+=code_req;
    id = id<<5;
    res+=id;
    
    return htons(res);
}

char * message_inscription_server(uint16_t code_req, uint16_t id, uint16_t numfil, uint16_t nb){
    char * res = malloc(6 * sizeof(char));
    //remplir l'entête
    ((uint16_t *)res)[0] = entete_message(code_req,id);
    //les autres champs
    ((uint16_t *)res)[1] = htons(numfil);
    ((uint16_t *)res)[2] = htons(nb);
    return res;
}
