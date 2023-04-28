#ifndef SERVEUR_H
#define SERVEUR_H

#include "inscrits.h"
#include "fils.h"

#define SIZE_MESS_SERV 6

/**
 * @brief recoit les 2 premiers octects (l'entête)
 * et la renvoie
 * 
 * @param sock la socket du client 
 * @return u_int16_t l'entête reçue ou -1 en cas de problèmes
 */
uint16_t lire_entete(int sock);

/**
 * @brief lit le pseudo de 10 octets et le renvoie
 * 
 * @param sock la socket du client
 * @return char* le pseudo avec '\0' à la fin
 */
char * lire_pseudo(int sock);

/**
 * inscrit le client et renvoie au client le message correspondanr avec l'id atttribué
 * @return int 1 en cas de succès, 0 en cas d'échec 
*/
int inscrire_client(int sock, inscrits_t * inscrits);

/**
 * envoie un message au client pour indiquer qu'il y a eu une erreur
*/
void envoie_message_erreur(int sock);


/**
 * lit le message restant du client et poste le billet puis envoie la confirmation au client
 * @return numéro de fil ou 0 en cas d'echec
*/
int poster_un_billet(int sock,inscrits_t * inscrits,fils_t * fils, uint16_t id);

int demander_des_billets(int sock,inscrits_t *inscrits,fils_t * filst,uint16_t id);


int lire_jusqua_datalen(int sock, uint16_t *numfil, uint16_t *nb, uint8_t *datalen);

int lire_data(int sock, uint8_t datalen, char *data);
int recevoir_fichier(int sock, inscrits_t* inscrits, fils_t* filst, uint16_t id);
#endif