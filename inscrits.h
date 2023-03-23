#ifndef INSCRITS_H
#define INSCRITS_H

#include <arpa/inet.h>

#define TRUE 1
#define FALSE 0

#define LONG_INIT 10
#define LEN_PSEUDO 10

typedef struct inscrit_t{
    uint16_t id;
    char pseudo[11]; //10 octets pour le pseudo + '\0' 
}inscrit_t;

typedef struct inscrits_t{
    inscrit_t * inscrits; //tableau de clients inscrits
    uint16_t nb_inscrits; //nombre de clients inscrits, id du suivant est `nb_inscrits+1`
    uint16_t capacite; //longueur effective du tableau 
}inscrits_t;


/**
 * alloue une struct inscrits de capacité LONG_INIT
 * @return la structure créee ou NULL en cas d'erreur
 */
inscrits_t * creer_inscrits_t ();


/**
 * inscrit l'utilisateur avec pseudo `pseudo` et lui attribue
 * un unique id qui est `nb_inscrits+1` (entre 1 et 2047) codé sur 11 bits
 * 
 * @param pseudo pseudo du client à inscrire sur 10 avec complétion de caractères #
 *  si le pseudo fait moins de 10 caractères 
 * @return uint16_t l'Id du client inscrit ou -1 en cas d'erreur malloc ou 0 si (nb_inscrits==2047 'plus de place')
 */
int add_user(inscrits_t * ins, char * pseudo);

/**
 * vérifie s'il existe un client parmi la liste des inscrit qui a comme id
 * `id`, le cas echéant, met son pseudo dans pseu
 * 
 * @param ins liste des inscrits
 * @param id_client le id à chercher
 * @return TRUE s'il y a un client inscrit avec id `id`, FALSE sinon 
 */
int est_inscrit(inscrits_t * ins, u_int16_t id_client, char * pseu);

/**
 * libère la mémoire allouée pour stocker la liste des inscrits
 * 
 * @param ins la liste des iscrits
 */
void free_list_inscrits(inscrits_t * ins);

#endif