
#ifndef MESSAGES_SERVEUR_H
#define MESSAGES_SERVEUR_H

#include <arpa/inet.h>

/**
 * @param code_req code de la requête du client, codé sur 5 bits (entre 0 et 31)
 * @param id id unique du client attribué par le serveur, codé sur 11 bits (entre 0 et 2047)
 * @return entête pour débuter le message au format big-endian
 */
uint16_t entete_message(uint16_t code_req, uint16_t id);

/**
 * @param code_req code de la requête d'inscription : 1 
 * @param id id du client
 * @param numfil numéro de fil : 0
 * @param nb numéro de port ou nombre de messages :0
 * 
 * @return le message du serveur au bon format
*/
char * message_inscription_server(uint16_t code_req, uint16_t id, uint16_t numfil, uint16_t nb);

#endif