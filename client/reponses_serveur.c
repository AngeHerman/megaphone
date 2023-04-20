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

u_int16_t reponse_inscription(uint16_t * rep){
    uint8_t cod_req;
    uint16_t id;
    u_int16_t numfil = ntohs(rep[1]);
    u_int16_t nb = ntohs(rep[2]);
    u_int16_t entete = ntohs(rep[0]);
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

uint16_t reponse_poster_billet(uint16_t * rep){
    uint8_t cod_req;
    uint16_t id;
    u_int16_t numfil = ntohs(rep[1]);
    u_int16_t nb = ntohs(rep[2]);
    u_int16_t entete = ntohs(rep[0]);
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

u_int16_t reponse_derniers_billets(u_int16_t * rep){
	uint8_t cod_req;
	uint16_t id;
	u_int16_t numfil = ntohs(rep[1]);
	u_int16_t nb = ntohs(rep[2]);
	u_int16_t entete = ntohs(rep[0]);
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

int reponse_abonnement(char *rep,char *addr,uint16_t *port){
    uint8_t cod_req;
	uint16_t id;
	u_int16_t numfil = ntohs(((uint16_t *)rep)[1]);
	u_int16_t port_multidiff = ntohs(((uint16_t *)rep)[2]);
	u_int16_t entete = ntohs(((uint16_t *)rep)[0]);
	u_int16_t masque = 0b0000000000011111;
	cod_req = entete & masque;
	id = (entete & ~masque) >> 5;
    if(cod_req!= CODE_REQ_ABONNEMENT_FIL){
		fprintf(stderr, "reponse de abonnement erroné et le code reçu est %d\n",cod_req);
		return 0;
	}
    memmove(addr,rep+6,16);
    uint16_t *addr_u;
    for(int i = 0; i < 8; i++ ){
        addr_u[i] = ntohs( ((uint16_t *)addr)[i] );
    }
    memmove(addr,addr_u,16);
    *port = port_multidiff;
    return 1;

}