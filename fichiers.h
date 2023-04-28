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

typedef struct bloc{
    uint16_t num_bloc;
    char* data;
    int data_len;
    struct bloc* next;
}bloc_t;

typedef struct fic{
    int num_dernier_bloc;
    int nb_bloc_recus;
    uint16_t id;
    bloc_t * blocs;
    struct fic* next;
}fic_t;

typedef struct temp_fic{
    fic_t * first;
}fics_t;



int transmission_fichiers(int sock_udp);
file_list_t * init_file_list();
fics_t * init_fics();
int annoncer_ecoute_pour_recevoir_fichier(int sock, uint16_t numfil, uint8_t id, uint16_t port);
int add_id_file(uint16_t id, char* name_file, uint16_t numfil);

#endif