#ifndef SERVEUR_H
#define SERVEUR_H

#include <arpa/inet.h>

/**
 * @param rep buffer qui contient l'entete du message du client
 * 
 * @return le codereq du message 
*/
u_int8_t codereq_recu(uint16_t rep);

#endif