#ifndef CLIENT_H
#define CLIENT_H

#include "buf.h"

#define NB_OCTECS_DERNIERS_MESSAGE_JUSQUA_DATALEN 23 //Numfil (2 otects)+origine(10)+pseudo(10)+datalen(1)

/* Affiche l'adresse du server ainsi que le port sur le client s'est connecté */
void affiche_adresse(struct sockaddr_in6 *adr);

/* Tente de connecter le client au sserveur puis renvoie un entier spécifiant si la connexion a réussi ou non */
int get_server_addr(char* hostname, char* port, int * sock, struct sockaddr_in6** addr, int* addrlen);

/*Demande au serveur les derniers billets selon les parametres*/
int demande_dernier_billets(int fd_sock,u_int16_t id_client,uint16_t numfil, uint16_t nb,buf_t* buffer);

/*Demande les n derniers billets de tous les fils*/
int demande_dernier_billets_tous_les_fils(int sock,u_int16_t id_client,buf_t* buffer);

int demande_inscription(int sock, char * pseudo);

u_int16_t id_attribue(u_int16_t * mess_server);

void test();

#endif