#ifndef CLIENT_H
#define CLIENT_H
#include <arpa/inet.h>

#define NB_OCTECS_DERNIERS_MESSAGE_JUSQUA_DATALEN 23 //Numfil (2 otects)+origine(10)+pseudo(10)+datalen(1)
#define NB_OCTECS_REPONSES_ABONNEMENT 22//code_req et ID (2 otects)+Numfil (2 otects)+nb (2) + adresse(16)
#define NB_OCTECS_MESSAGE_SERVEUR 6
#define TAILLE_MAX_AJOUT_FICHIER 33554432 //en octets
#define TAILLE_MAX_STRING 4096 //taille maximale des char *


/**
 *  @brief  l'adresse du server ainsi que le port sur le client s'est connecté 
*/
void affiche_adresse(struct sockaddr_in6 *adr);

/**
 *  @brief  Tente de connecter le client au sserveur puis renvoie un entier spécifiant si la connexion a réussi ou non 
*/
int get_server_addr(char* hostname, char* port, int * sock, struct sockaddr_in6* addr, int* addrlen);

/**
 *  @brief Demande au serveur les derniers billets selon les parametres 
*/
int demande_dernier_billets(int fd_sock,uint16_t id_client,uint16_t numfil, uint16_t nb);

/**
 * @brief le client et renvoie l'id attribué par le serveur ou 0 en cas d'échec 
*/
uint16_t demande_inscription(int sock, char * pseudo);

/**
 * @brief envoie un message au serveur pour poster le billet de texte data
 * @return 0 en cas d'échec, le numéro du fil sur lequel le billet a été posté sinon
*/
uint16_t poster_un_billet(int sock, uint16_t id, uint16_t num_fil, uint8_t datalen, char * data);

/**
 * abonne le client au fil numfil
 * @return 0 en cas d'échec, le numéro du fil sur lequel le billet a été posté sinon
*/
int demande_abonnement(int sock,u_int16_t id_client,uint16_t numfil);

/**
 * @brief lit la donnée à partir de la socket puis la stocke dans data
 * @param data là ou copier la données récupérée
 * @param taille la taille de données à lire
 * @param sock la socket
 * @return 0 en cas d'échec, 1 sinon
*/
int get_data(char *data,int taille,int sock);

/**
 * @brief ajoute le fichier nom_fichier au fil f
 * @return 0 en cas d'échec, le numéro du fil sur lequel le fichier a été posté sinon
*/
uint16_t ajouter_un_fichier(int sock, uint16_t id, uint16_t num_fil, uint8_t taille_nom_fichier, char * nom_fichier,char * hostname, char* file_path);

/**
 * @brief télécharge le fichier nom_fichier du fil f
 * @return 0 en cas d'échec, le numéro du fil sur lequel le fichier a été posté sinon
*/
uint16_t telecharger_un_fichier(int sock, uint16_t id, uint16_t num_fil, uint8_t taille_nom_fichier, char * nom_fichier);




#endif