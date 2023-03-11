#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "messages.h"


uint16_t entete_message(uint16_t code_req, uint16_t id){
    uint16_t res = 0;

    //déplacer de 5 bits vers la gauche
    id = id << 5;
    //insérer les 11 bits dans res
    res |= id;

    //insérer les 5 bits de id dans res
    res |= code_req;
    
    //convertir en format big-endian
    return htons(res);
}