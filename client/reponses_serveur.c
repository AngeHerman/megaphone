#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>

#include "reponses_serveur.h"
#include "messages_client.h"

uint16_t get_entete(char * rep){
	return ntohs(((uint16_t *)rep)[0]);
}

uint16_t get_num_fil(char * rep){
    return ntohs(((uint16_t *)rep)[1]);
}

uint16_t get_nb(char * rep){
    return ntohs(((uint16_t *)rep)[2]);
}

u_int16_t reponse_inscription(char * rep){
    uint8_t cod_req;
    uint16_t id;
    u_int16_t numfil = get_num_fil(rep);
	u_int16_t nb = get_nb(rep);
	u_int16_t entete = get_entete(rep);
    u_int16_t masque = 0b0000000000011111;
    cod_req = entete & masque;
    id = (entete & ~masque) >> 5;

    if(cod_req==31)
        return 0;

    if(cod_req!=1 || numfil!=0 || nb!=0){
        fprintf(stderr, "message de confirmation d'inscription erroné\n");
        return 0;
    }

    return id;
}


uint16_t reponse_poster_billet(char * rep){
    uint8_t cod_req;
    uint16_t id;
    u_int16_t numfil = get_num_fil(rep);
	u_int16_t nb = get_nb(rep);
	u_int16_t entete = get_entete(rep);
    u_int16_t masque = 0b0000000000011111;
    cod_req = entete & masque;
    id = (entete & ~masque) >> 5;

    if(cod_req==31)
        return 0;

    if(cod_req!=2 || nb!=0){
        fprintf(stderr, "message de confirmation erroné\n");
        return 0;
    }

    return numfil;
}

u_int16_t reponse_derniers_billets(char * rep){
	uint8_t cod_req;
	uint16_t id;
	u_int16_t numfil = get_num_fil(rep);
	u_int16_t nb = get_nb(rep);
	u_int16_t entete = get_entete(rep);
	u_int16_t masque = 0b0000000000011111;
	cod_req = entete & masque;
	id = (entete & ~masque) >> 5;


	if(cod_req!= CODE_REQ_DEMANDE_BILLETS){
		fprintf(stderr, "reponse de demande des derniers billets erroné et le code reçu est %d\n",cod_req);
		return 0;
	}
	printf("id est %u codereq est %u Numfil est %u et Nb est %u\n",id,cod_req,numfil,nb);
	return nb;
}


u_int8_t reponse_derniers_billets_datalen(char* rep){
	u_int16_t numfil = ntohs( ((uint16_t *)rep)[0]);
	char  origine[11];
	char pseudo[11];
	memmove(origine,rep+2,10);
	origine[10] = '\0';
	memmove(pseudo,rep+12,10);
	pseudo[10] = '\0';
	printf("Numfil : %u\n",numfil);
	printf("Origine : %s\n",origine);
	printf("Pseudo : %s\n",pseudo);
	uint8_t datalen = ((uint8_t *)rep)[22];
	return datalen;
}

int reponse_abonnement(char *rep,struct in6_addr *addr,uint16_t *port, uint16_t id_client, uint16_t num_fil){
	u_int16_t numfil_rec = ntohs(((uint16_t *)rep)[1]);
	u_int16_t entete = ntohs(((uint16_t *)rep)[0]);
	u_int16_t masque = 0b0000000000011111;
	uint8_t cod_req = entete & masque;
	uint16_t id = (entete & ~masque) >> 5;
	
    if(cod_req!= CODE_REQ_ABONNEMENT_FIL || id!= id_client || num_fil != numfil_rec){
		fprintf(stderr, "reponse de abonnement erroné et le code reçu est %d\n",cod_req);
		return 0;
	}
	*port = ((uint16_t *)rep)[2];
	*addr = *((struct in6_addr*) (rep+6));
    return 1;
}

uint16_t reponse_ajout_fichier(char * rep,int id_user){
	u_int16_t numfil = get_num_fil(rep);
	u_int16_t port = get_nb(rep);
	u_int16_t entete = get_entete(rep);
	u_int16_t masque = 0b0000000000011111;
	uint8_t cod_req = entete & masque;
	uint16_t id = (entete & ~masque) >> 5;

	if(cod_req!= CODE_REQ_AJOUT_FICHIER || id!= id_user){
		fprintf(stderr, "reponse ajout fichier est erroné et le code reçu est %d\n",cod_req);
		return 0;
	}
	return port;
}

int notification(char * mess_notif, uint16_t * numfil, char * pseudo, char * data){
	uint16_t entete = ntohs(((uint16_t *)mess_notif)[0]);
	u_int16_t masque = 0b0000000000011111;
	uint8_t cod_req = entete & masque;
	uint16_t id = (entete & ~masque) >> 5;

	if(cod_req!= CODE_NOTIFICATION || id!=0){
		fprintf(stderr, "notification erronée, le code reçu est %d et l'id est %d\n",cod_req, id);
		return 0;
	}
	*numfil = ntohs(((uint16_t *)mess_notif)[1]);
	memmove(pseudo, mess_notif+4, 10);
	memmove(data, mess_notif+14, 20);
	return 1;
}

uint16_t reponse_telechargement_fichier(char * rep,uint16_t id_user,uint16_t numfil_user,uint16_t port){
	u_int16_t numfil = get_num_fil(rep);
	u_int16_t nb = get_nb(rep);
	u_int16_t entete = get_entete(rep);
	u_int16_t masque = 0b0000000000011111;
	uint8_t cod_req = entete & masque;
	uint16_t id = (entete & ~masque) >> 5;

	if(cod_req!= CODE_REQ_TELECHARGER_FICHIER || id!= id_user || numfil_user!= numfil || port!= nb){
		fprintf(stderr, "reponse ajout fichier est erroné et le code reçu est %d\n",cod_req);
		return 0;
	}
	return 1;
}
