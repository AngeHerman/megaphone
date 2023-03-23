#ifndef CLIENT_H
#define CLIENT_H
#include <arpa/inet.h>
#include "buf.h"

#define NB_OCTECS_DERNIERS_MESSAGE_JUSQUA_DATALEN 23 //Numfil (2 otects)+origine(10)+pseudo(10)+datalen(1)

/* Affiche l'adresse du server ainsi que le port sur le client s'est connecté */
void affiche_adresse(struct sockaddr_in6 *adr);

/* Tente de connecter le client au sserveur puis renvoie un entier spécifiant si la connexion a réussi ou non */
int get_server_addr(char* hostname, char* port, int * sock, struct sockaddr_in6** addr, int* addrlen);

/*Demande au serveur les derniers billets selon les parametres*/
int demande_dernier_billets(int fd_sock,uint16_t id_client,uint16_t numfil, uint16_t nb,buf_t* buffer);

/*Demande les n derniers billets de tous les fils*/
int demande_dernier_billets_tous_les_fils(int sock,u_int16_t id_client,buf_t* buffer);

/**
 * inscrit le client et renvoie l'id attribué par le serveur ou 0 en cas d'échec 
*/
uint16_t demande_inscription(int sock, char * pseudo);

/**
 * véréfie que la réponse du serveur à l'inscription du client
 * respecte le bon format et renvoie l'id attribué par le serveur
 * 
 * @param res tableau de 3 uint16_t contanant le message du serveur
 * @return uint16_t l'id du client ou 0 en cas d'échec
*/
uint16_t reponse_inscription(uint16_t * rep);

/**
 * envoie un message au serveur pour poster le billet de texte data
 * @return 0 en cas d'échec, le numéro du fil sur lequel le billet a été posté sinon
*/
uint16_t poster_un_billet(int sock, uint16_t id, uint16_t num_fil, uint8_t datalen, char * data);

#endif