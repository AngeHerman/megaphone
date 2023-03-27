
#ifndef MESSAGES_SERVEUR_H
#define MESSAGES_SERVEUR_H

#include <arpa/inet.h>

#define LEN_PSEUDO 10
/**
 * @param code_req code de la requête du client, codé sur 5 bits (entre 0 et 31)
 * @param id id unique du client attribué par le serveur, codé sur 11 bits (entre 0 et 2047)
 * @return entête pour débuter le message au format big-endian
 */
uint16_t entete_message(uint16_t code_req, uint16_t id);

/**
 * code_req = 1, id = id_user, num_fil = 0, nb = 0, pour envoyer une répose à une inscription
 * code_req = 2, id = id_user, num_fil = num demandé ou num du nouveau fil, pour confirmer l'envoie du billet
 * code_req = 3, id = id_user, num_fil = f, nb = n, pour annoncer l'envoie des billets
 *  - num_fil = num_fil_demandé si >0 sinon il vaut le nb de fils en cours dans la réponse 
 *  - pour nb:
 *      *si f>0: 
 *          - nb = nb messages contenus dans le fil si n est supérieur à ce nombre ou n = 0 
 *          - n sinon\n
 *      *sinon
 *          - nb = nb total de messages que le serveur va envoyer (somme de nombre de messages envoyés de chaque fil)
 *              !nb messages envoyé de chaque fil suit la même règle que dans le cas où f>0
 *  
 * @param code_req code de la requête  
 * @param id id du client
 * @param numfil numéro de fil 
 * @param nb numéro de port ou nombre de messages
 * 
 * @return le message du serveur au bon format
*/
char * message_server(uint16_t code_req, uint16_t id, uint16_t numfil, uint16_t nb);

/**
 * @brief construit un message pour envoyer un billet au client au bon format
 * 
 * @param numfil le numéro du fil auquel appartient le billet
 * @param origine pseudo de l’initiateur du fil
 * @param pseudo pseudo de celui qui a posté le billet
 * @param datalen longeur du message
 * @param data texte du billet
 * @return char* le message a envoyer au client 
 */
char * message_billet(uint16_t numfil, char* origine, char* pseudo, uint8_t datalen, char* data);


/**
 * @brief renvoie le code de la requête à partir de l'entête
 * 
 * @param entete l'entête
 * @return uint8_t le code de la requête
 */
uint8_t get_code_req(uint16_t entete);

/**
 * @brief rencoie l'id à partir de l'entête
 * 
 * @param entete l'entête
 * @return uint16_t l'id du client à l'origine de la requête
 */
uint16_t get_id_requete(uint16_t entete);

#endif