#ifndef MESSAGES_CLIENT_H
#define MESSAGES_CLIENT_H

#include <arpa/inet.h>

#define LEN_PSEUDO 10
#define LEN_MESS_INSCR 12 //entête de deux octets + 10 octets pour le pseudo
#define LEN_MESS_CLIENT 7// entête de deux octets + 2 octets numfil + 2 octets nb + 1 octet datalen
#define TAILLE_PAQUET_UDP 512

#define CODE_REQ_P0STER_BILLET 2 //code_req pour poster un billet
#define CODE_REQ_DEMANDE_BILLETS 3 //code_req pour la demande des billets
#define CODE_REQ_ABONNEMENT_FIL 4 //code_req pour l'abonnement à un
#define CODE_NOTIFICATION 4 //code pour les notifications envoyées par le serveur
#define CODE_REQ_AJOUT_FICHIER 5 //code_req pour l'ajout de fichier
#define CODE_REQ_TELECHARGER_FICHIER 6 //code_req pour le telechargement de fichier



/**
 * client : 
 * code_req = 1 , id = 0 pour inscrire le client
 * code_req = 2 , id = id_user pour poster un billet 
 * code_req = 3 , id = id_user pour demander la liste des n derniers billets
 * code_req = 4 , id = id_user pour s'abonner à un fil
 * code_req = 5 , id = id_user pour ajouter un fichier à un fil
 * code_req = 6 , id = id_user pour demander à télécharger un fichier 
 * 
 * serveur :
 * code_req = 1 , id = id attribué au client lors de l'inscription
 * code_req = 2 , id = id_user pour confirmer que le billet a été posté 
 * code_req = 3 , id = id_user pour répondre à une demande des n derniers billets
 * code_req = 4 , id = id_user pour répondre à une demande d'abonnement
 * code_req = 4 , id = 0 pour les notifications
 * code_req = 5 , id = id_user pour répodre à une demande d'ajout de fichier
 * code_req = 6 , id = id_user pour répondre à une demande de téléchargement de fichier
 * code_req = 31 , id = 0 en cas d'erreur
 * @param code_req code de la requête du client, codé sur 5 bits (entre 0 et 31)
 * @param id id unique du client attribué par le serveur, codé sur 11 bits (entre 0 et 2047)
 * @return entête pour débuter le message au format big-endian
 */
uint16_t entete_message(uint16_t code_req, uint16_t id);

/**
 * renvoie un message d'inscription avec le pseudo `pseudo`
 * il faut free le message à la fin de l'utilisation
 * 
 * @param pseudo chaine de caractère (avec '\0' à la fin) contenant le pseudo du clien (10 caractères maximum)
*/
char * message_inscription_client(char * pseudo);

/**
 * code_req = 2 , id = id_user , numfil = f, nb = 0, datalen = strlen(texte), data = texte pour poster un billet sur le fil f avec le texte `texte`
 * code_req = 3 , id = id_user , numfil = f, nb = n, datalen = 0, data = "" pour demander la liste des n derniers billets
 *  -n=0 -> tous les billets du fil
 *  -f=0 -> liste des n derniers billets de chaque fil
 *  -n=0 et f=0 -> liste de tous les billets de chaque fil
 * code_req = 4 , id = id_user, numfil = f, nb = 0, datalen = 0, data = "" pour s'abonner à un fil f
 * code_req = 5 , id = id_user, numfil = f, nb = 0, datalen = strlen(name_file), len = name_file pour ajouter un fichier name_file à un fil f
 * code_req = 6 , id = id_user, numfil = f, nb = num_port, datalen = strlen(name_file), len = name_file pour demander à télécharger un fichier apparaissant sur un fil f
 *
 * @param code_req code de la requête du client
 * @param id id du client
 * @param numfil numéro de fil
 * @param nb numéro de port ou nombre de messages
 * @param datalen la taille du texte du message
 * @param data le texte du message
 * 
 * @return le message du client au bon format
*/
char * message_client(uint16_t code_req, uint16_t id, uint16_t numfil, uint16_t nb, uint8_t datalen, char * data);


/**
 *
 * @param code_req code de la requête du client
 * @param id id du client
 * @param numbloc le numéro du bloc
 * @param datalen la taille de data
 * @param data la donnée elle même
 * 
 * @return le message du client au bon format
*/
char * message_client_udp(uint16_t code_req, uint16_t id,uint16_t numbloc, int datalen, char * data);





/**
 * @param id id du client
 * @param numfil numéro de fil
 * @param nb numéro de port ou nombre de messages
 * 
 * @return le message du client au bon format
*/
char * message_dernier_billets(uint16_t id, uint16_t numfil, uint16_t nb);


/**
 * @param id id du client
 * @param numfil numéro de fil
 * 
 * @return le message du client au bon format
*/
char * message_abonnement_fil(uint16_t id, uint16_t numfil);






#endif

