#ifndef CLIENT_H
#define CLIENT_H

/* Affiche l'adresse du server ainsi que le port sur le client s'est connecté */
void affiche_adresse(struct sockaddr_in6 *adr);

/* Tente de connecter le client au sserveur puis renvoie un entier spécifiant si la connexion a réussi ou non */
int get_server_addr(char* hostname, char* port, int * sock, struct sockaddr_in6** addr, int* addrlen);

void test();

#endif