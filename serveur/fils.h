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
    struct sockaddr_in6 addr_multi; // la structure du multidiffuseur
    int a_abonne;//1 si le fil a au moins un abonné, 0 sinon
    int derniere_notif;//le numero du dernier billet envoyé par une notification, 0 si aucun billet n'a encore été envoyé
}fil_t;

typedef struct fils_t{
    uint16_t nb_fils; // nombre de fils existants, le fil suivant aura `nb_fils+1` pour `num_fil`
    int capacite; // longueur effective du tableau de fils
    fil_t * fils; // tableau de fils
}fils_t;

typedef struct info_multi{
    fils_t * fils;
    uint16_t numfil;
    struct sockaddr_in6 addr_multi;
}info_multi;

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


/**
 * @brief Get the messages object
 * 
 * @param fils la liste de fils
 * @param numfil le numéro de fil ou =0 pour demander des messages sur tous les fils
 * @param nb nombre de messages voulu ou =0 pour demander tous les messages 
 * @param messages[out] la liste de messages à envoyer au client
 * @param numfil_rep[out] numfil si numfil>0 sinon il vaut le nb de fils en cours dans la réponse 
 * @param nb_rep[out] nombre de messages à envoyer
 * @return int 1 en cas de succès; 0 en cas d'échec 
 */
int get_messages(fils_t * fils, uint16_t numfil, uint16_t nb, char*** messages, uint16_t* numfil_rep, uint16_t* nb_rep);


struct sockaddr_in6 get_addr_multi(fils_t * fils, uint16_t numfil);

void free_messages_billets(char** messages, uint16_t nb_mess);
fils_t * copy_list_fils(fils_t * fs);
fil_t copy_fil(fil_t  f);
billet_t copy_billet(billet_t b);


int abonner_fil(fils_t * fils, uint16_t numfil, struct sockaddr_in6 * addr_multi);
void * multi_diffusion(void * args);
#endif