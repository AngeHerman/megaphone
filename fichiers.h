/*Fonction pour l'envoi de fichiers entre client et serveur*/
#ifndef FICHIERS_H
#define FICHIERS_H

#include "serveur/inscrits.h"
#include "serveur/fils.h"

#define TAILLE_PAQUET_UDP 512
#define CODE_REQ_AJOUT_FICHIER 5 //code_req pour l'ajout de fichier
#define SIZE_MESS_SERV 6
#define TAILLE_MAX_STRING 4096 //taille maximale des char *
#define TAILLE_MAX_AJOUT_FICHIER 33554432 //en octets
#define CHEMIN_FICHIER_CLIENT "client/fichiers/"

typedef struct bloc{
    uint16_t num_bloc;
    char* data;
    int data_len;
    struct bloc* next;
}bloc_t;

typedef struct fic{
    int num_dernier_bloc; //le numéro du dernier bloc (taille<LEN_PAQUET)
    int nb_bloc_recus; //le nombre de paquets reçus
    uint16_t numfil; //numéro de fil du fichier
    uint16_t id; //id du client qui a envoyé le fichier
    char * namefile; //le nom du fichier
    bloc_t * blocs; //liste chaînée des blocs constituant le fichier
}fic_t;


/**
 * @brief 
 * 
 * @param sock_udp socket udp
 * @param name_file nom du fichier à envoyer
 * @param id identifiant du client
 * @param numfil numéro de fil sur lequel poster le fichier
 * 
 * @return int taille du fichier reçu, ou -1 en cas d'erreur 
*/
int reception_par_paquets_de_512(int sock_udp, char* name_file, uint16_t id, uint16_t numfil, int is_client);

/**
 * @brief Envoie le fichier par paquets de 512
 * @return 0 en cas d'échec, 1 si tout s'est bien passé
*/
int envoi_par_paquets_de_512(int fd, int sock,int id,int taille_fic, struct sockaddr_in6 addr, int adrlen);

/**
 * @brief ajoue le fichier de chemin file_path sur le port port et adresse de nom hostname
 * @return 0 en cas d'échec, 1 si le fichier a été envoyé
*/
int envoi_fichier(uint16_t id, uint16_t port,char * file_path,char * hostname, int is_client,struct in6_addr addr_client_recup);

/**
 * @brief Renvoie la taille du fichier file_name
 * @return 0 en cas d'échec, la taille du fichier sinon
*/
long int taille_fichier(char *file_name);



#endif