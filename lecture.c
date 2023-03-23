#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "lecture.h"

buf_t * creer_buf_t (int size){
    buf_t * res = malloc(sizeof(buf_t));
    if(res==NULL){
        perror("malloc");
        return NULL;
    }
    res->current = 0;
    res->expected_size = size;
    res->buf = malloc(res->expected_size * (sizeof(char))); 
    if(res->buf==NULL){
        perror("malloc");
        free(res);
        return NULL;
    }
    memset(res->buf, 0, sizeof(res->buf));
    return res;
}


int read_msg(int sock, buf_t* buf){
    int size_recv=0;
    
    while(((size_recv = recv(sock, buf->buf+buf->current, buf->expected_size-buf->current, 0)) < buf->expected_size - buf->current) && size_recv>0 ){
        buf->current += size_recv;
    }
    buf->current += size_recv;
    if(buf->current!=buf->expected_size)
        return 0;
    return 1;
}


void free_buf(buf_t * buf){
    free(buf->buf);
    free(buf);
}
