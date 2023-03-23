#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "buf.h"

buf_t * creer_buf_t (){
    buf_t * res = malloc(sizeof(buf_t));
    if(res==NULL){
        perror("malloc");
        return NULL;
    }
    memset(res,0,sizeof(buf_t));
    res->cur = 0;
    res->size = 0;
    res->buf = malloc((SIZE_BUF +1) * (sizeof(char))); 
    if(res->buf==NULL){
        perror("malloc");
        free(res);
        return NULL;
    }
    memset(res->buf, 0, sizeof(res->buf));
    return res;
}


int read_msg(int sock,buf_t* buf,char *msg_buf, int nb_octets_voulu){
    int size_recv;
    //pur est la position ou il faudra commencer à compter les octecs
    int cur_depart = buf->cur;
    while((size_recv = recv(sock, buf->buf+buf->cur, SIZE_BUF, 0)) > 0){
        printf("Tour et cur est %d\n",buf->cur);
        buf->size += size_recv;

        printf("Buff ici est : <%s> \n",buf->buf);

        //On continue tant que les octetcs obtenu jusque là sont < au nombre voulu
        if(( (buf->cur + size_recv) - cur_depart) < nb_octets_voulu){
            printf("On continue\n");
            buf->cur += size_recv;
            continue;
            printf("Apres continue\n");

        }
        printf("Trouve et buf-cur vaut vaut <%d>\n",buf->cur);
        //On copie les octets
        memmove(msg_buf,buf->buf+cur_depart,nb_octets_voulu);
        msg_buf[nb_octets_voulu] = '\0';
        //la nouvelle position courante sera juste la position courante de départ + les octes copiés
        buf->cur = cur_depart+ nb_octets_voulu;
        printf("buf cur est %d et size est %d \n",buf->cur,buf->size);
        return 0;
    }
    printf("Echec, Pas assez d'octecs reçu\n");
    return -1;
}

int read_buf(int sock,buf_t* buf,char *msg_buf, int nb_octets_voulu){
    printf("----buf cur est %d et size est %d \n",buf->cur,buf->size);

    //Quand on a déja les octets suffisant
    if( (buf->size - buf->cur) >= nb_octets_voulu){
        memmove(msg_buf,buf->buf+buf->cur,nb_octets_voulu);
        msg_buf[nb_octets_voulu] = '\0';
        buf->cur += nb_octets_voulu;
        return 0;
    }else{
        return read_msg(sock,buf,msg_buf,nb_octets_voulu);
    }
}