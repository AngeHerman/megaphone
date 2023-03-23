#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "serveur.h"
#include "lecture.h"


uint16_t lire_entete(int sock){
    buf_t * buf = creer_buf_t(2);
    if(buf==NULL)
        return -1;
    int r = read_msg(sock, buf);
    if(r==0){
        free(buf);
        return -1;
    }
    uint16_t entete = ntohs(*((uint16_t *)buf->buf));
    free(buf);
    return entete;
}

char * lire_pseudo(int sock){
    buf_t * buf = creer_buf_t(10);
    if(buf==NULL)
        return NULL;
    int r = read_msg(sock, buf);
    if(r==0){
        free(buf);
        return NULL;
    }
    char * pseudo = malloc(11*sizeof(char));
    if(pseudo==NULL)
        return NULL;
    memmove(pseudo, buf->buf, 10);
    free(buf);
    return pseudo;
}