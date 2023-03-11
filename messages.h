#ifndef MESSAGES_H
#define MESSAGES_H

#include <arpa/inet.h>

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



#endif