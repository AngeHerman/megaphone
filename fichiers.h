/*Fonction pour l'envoi de fichiers entre client et serveur*/
#ifndef FICHIERS_H
#define FICHIERS_H

#include "serveur/inscrits.h"
#include "serveur/fils.h"
#define LEN_PAQUET 512 

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
 * @brief envoie au client le numéro de port sur lequel le serveur recevra les paquets UDP
 * 
 * @param sock socket de communication tcp
 * @param numfil numfil demandé par le client
 * @param id identifiant du client
 * @param port le numéro de port sur lequel le serveur recevra les paquets UDP

 * @return int 1 en cas de succes, 0 en cas de d'erreur
 */
int annoncer_ecoute_pour_recevoir_fichier(int sock, uint16_t numfil, uint8_t id, uint16_t port);

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
int transmission_fichiers(int sock_udp,char * name_file,uint16_t id, uint16_t numfil);

#endif