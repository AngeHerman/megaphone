#ifndef REPONSES_SERVEUR_H
#define REPONSES_SERVEUR_H


/**
 * @brief verifie que la réponse du serveur à l'inscription du client respecte le bon format et renvoie 
 * l'id attribué par le serveur
 * 
 * @param rep tableau de 3 uint16_t contanant le message du serveur
 * @return uint16_t l'id du client ou 0 en cas d'échec
*/
uint16_t reponse_inscription(uint16_t * rep);

/**
 * @brief verifie que la réponse du serveur respecte le bon format et renvoie le numero du fil
 * @param rep tableau de 3 uint16_t contanant le message du serveur
 * @return uint16_t le numfil ou 0 en cas d'échec
*/
uint16_t reponse_poster_billet(uint16_t * rep);

/**
 * @param rep la reponse du serveur à décoder
 * @return le nombre de message à venir
*/
u_int16_t reponse_derniers_billets(u_int16_t * rep);

/**
 * @param rep la reponse du serveur jusqu'à datalen à décoder
 * 
 * @return datalen la taille du message qui arrive
*/
u_int8_t reponse_derniers_billets_datalen(char* rep);

/**
 * @param rep la reponse du serveur à décoder
 * @param addr la variable où sera stockée l'adresse de multidiffusion
 * @return l'adresse d'abonnement envoyé par le serveur
*/
int reponse_abonnement(char *rep,char *addr,uint16_t *port);

/**
 * @brief verifie que la réponse du serveur respecte le bon format et renvoie le numero de port UDP
 * @param rep la reponses du serveur
 * @return uint16_t le numéro de port UDP ou 0 en cas d'échec
*/
uint16_t reponse_ajout_fichier(char * rep);


#endif