#ifndef FILS_H
#define FILS_H

#include <arpa/inet.h>

typedef struct billet_t{
    char pseudo[11]; //pseudo de celui qui a posté le billet
    uint8_t data_len; //longueur du billet
    char* data; //le texte du billet

}billet_t;

typedef struct fil_t{
    uint16_t num_fil; //le numéro du fil de 1 à 65535
    char origine[11]; //pseudo de l'initiateur du fil
    billet_t * billets; //tableau des billets postés sur le fil
    int nb_billets; // le nombre de billets sur le fil
    int capacite; // capacite du tableau billets
}fil_t;

typedef struct fils_t{
    uint16_t nb_fils; // nombre de fils existants, le fil suivant aura `nb_fils+1` pour `num_fil`
    int capacite; // longueur effective du tableau de fils
    fil_t * fils; // tableau de fils
}fils_t;

/**
 * @brief crée une liste de fils vide de capacité CAP_FILS_INIT
 * @return fils_t* liste créée ou NULL en cas d'erreur
 */
fils_t * creer_list_fils();

/**
 * @brief libère les ressources allouées pour la liste des fils
 * 
 * @param fils la liste des fils
 */
void free_fils(fils_t * fs);

/**
 * @brief libère les ressources allouées pour les billets d'un fil 
 * 
 * @param fil le fil
 */
void free_fil(fil_t fil);

/**
 * @brief crée un nouveau fil de capacité CAP_FIL_INIT
 * 
 * @param fils liste des fils
 * @param orig initiateur du fil
 * @return fil_t* le nouveau fil crée ou NULL en cas d'erreur
 */
fil_t * ajouter_nouveau_fil(fils_t * fs, char * orig);

/**
 * @brief poster un billet sur le fil `fil`
 * 
 * @param fil le fil auquel on veut ajouter le billet
 * @param pseu pseudo de celui qui a posté le billet
 * @param len la longueur du billet
 * @param text_billet le texte du billet
 * @return int TRUE en cas de succès, FALSE en cas d'echec
 */
int ajouter_billet(fil_t * fil, char * pseu, uint8_t len, char* text_billet);

/**
 * @brief poster un billet sur le fil numéro num_f
 * 
 * @param fs la liste des fils
 * @param num_f le numéro du fil auquel on veut ajouter le billet
 * @param pseu pseudo de celui qui a posté le billet
 * @param len la longueur du billet
 * @param text_billet le texte du billet
 * @return int TRUE en cas de succès, FALSE en cas d'echec
 */
int ajouter_billet_num(fils_t* fs, uint16_t num_f, char * pseu, uint8_t len, char* text_billet);



#endif