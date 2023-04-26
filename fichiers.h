#ifndef FICHIERS_H
#define FICHIERS_H

#include "serveur/inscrits.h"
#include "serveur/fils.h"
#define LEN_PAQUET 512 


typedef struct id_file{
    uint16_t id;
    uint16_t numfil;
    char * name;
    struct id_file * next;
}id_file_t;

typedef struct file_list{
    id_file_t * first;
}file_list_t;


int recevoir_fichier(int sock, inscrits_t* inscrits, fils_t* filst, uint16_t id, uint16_t port);
int transmission_fichiers(int sock_udp);
file_list_t * init_file_list();

#endif