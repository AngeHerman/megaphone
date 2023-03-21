#ifndef CLIENT_H
#define CLIENT_H

/* Affiche l'adresse du server ainsi que le port sur le client s'est connecté */
void affiche_adresse(struct sockaddr_in6 *adr);

/* Tente de connecter le client au sserveur puis renvoie un entier spécifiant si la connexion a réussi ou non */
int get_server_addr(char* hostname, char* port, int * sock, struct sockaddr_in6** addr, int* addrlen);

/*Demande au serveur les derniers billets selon les parametres*/
int demande_dernier_billets(int fd_sock,u_int16_t id_client,uint16_t numfil, uint16_t nb);

/*Demande les n derniers billets de tous les fils*/
int demande_dernier_billets_tous_les_fils(int fd_sock,u_int16_t id_client);

int demande_inscription(int fd_sock, char * pseudo);
u_int16_t id_attribue(u_int16_t * mess_server);

void test();

#endif