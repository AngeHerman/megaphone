#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/*Verifie le codereq recu*/
u_int8_t codereq_recu(uint16_t rep){
    uint8_t cod_req;
    uint16_t id;
    u_int16_t entete = ntohs(rep);
    u_int16_t masque = 0b0000000000011111;
    cod_req = entete & masque;
    return cod_req;
}