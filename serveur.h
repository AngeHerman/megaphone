#ifndef SERVEUR_H
#define SERVEUR_H


/**
 * @brief recoit les 2 premiers octects (l'entête)
 * et la renvoie
 * 
 * @param sock la socket du client 
 * @return u_int16_t l'entête reçue ou -1 en cas de problèmes
 */
uint16_t lire_entete(int sock);

/**
 * @brief lit le pseudo de 10 octets et le renvoie
 * 
 * @param sock la socket du client
 * @return char* le pseudo avec '\0' à la fin
 */
char * lire_pseudo(int sock);

#endif