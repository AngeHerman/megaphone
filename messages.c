#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "messages.h"


uint16_t entete_message(uint16_t code_req, uint16_t id){
    uint16_t res = 0;

    res+=code_req;
    id = id<<5;
    res+=id;
    
    return htons(res);
}