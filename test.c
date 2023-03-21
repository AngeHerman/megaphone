#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "messages.h"
#include "inscrits.h"
#include "fils.h"

int main(){

    uint16_t entete_mess = entete_message(30,20);

    printf("%d\n",entete_mess);
    printf("%X, %X\n", ((char *)&entete_mess)[0],  ((char*)&entete_mess)[1] );
    entete_mess = ntohs(entete_mess);
    printf("%X, %X\n", ((char *)&entete_mess)[0],  ((char*)&entete_mess)[1] );

    u_int16_t x = 0x0102<<2;
    printf("%x,%x\n", ((char*) &x)[0], ((char*) &x)[1]);
    exit(0);

}