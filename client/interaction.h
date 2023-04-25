#ifndef INTERACTION_H
#define INTERACTION_H



/**
 * demande au client l'action qu'il veut effectuer et l'exécute
*/
int choix_client(int sock,char* hostname);


/**
 * demande le pseudo du client et fait l'inscription auprès du serveur
 * @return int 1 en cas de succès, et 0 en cas d'échec
*/
int inscrire_client(int sock);

/**
 * demande l'id, le numéro de fil, et le texte du billet à l'utilisateur
 * (num_fil = 0 pour poster sur un nouveau fil)
 * envoie le message pour poster le billet au serveur
 * @return int 1 en cas de succès, et 0 en cas d'échec
*/
int poster_billet(int sock);

/**
 * @return int 1 en cas de succès, et 0 en cas d'échec
*/
int abonnement_fil(int sock);

/**
 * @return int 1 en cas de succès, et 0 en cas d'échec
*/
int ajouter_fichier(int sock,char * hostname);

/**
 * @return int 1 en cas de succès, et 0 en cas d'échec
*/
int telecharger_fichier(int sock);

#endif