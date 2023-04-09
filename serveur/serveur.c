#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "serveur.h"
#include "../lecture.h"
#include "inscrits.h"
#include "fils.h"
#include "messages_serveur.h"

#define SIZE_MESS_SERV 6

uint16_t lire_entete(int sock)
{
    buf_t *buf = creer_buf_t(2);
    if (buf == NULL)
        return 0;
    int r = read_msg(sock, buf);
    if (r == 0)
    {
        free(buf);
        return 0;
    }
    uint16_t entete = ntohs(*((uint16_t *)buf->buf));
    free_buf(buf);
    return entete;
}

char *lire_pseudo(int sock)
{
    buf_t *buf = creer_buf_t(10);
    if (buf == NULL)
        return NULL;
    int r = read_msg(sock, buf);
    if (r == 0)
    {
        free_buf(buf);
        return NULL;
    }
    char *pseudo = (char *)malloc(11 * sizeof(char));
    if (pseudo == NULL)
        return NULL;
    memmove(pseudo, buf->buf, 10);
    free_buf(buf);
    return pseudo;
}

int inscrire_client(int sock, inscrits_t *inscrits)
{
    char *pseudo = lire_pseudo(sock);
    int id = add_user(inscrits, pseudo);
    if (id == 0 || id == -1)
    {
        if (id == 0)
            fprintf(stderr, "mémoire pleine");
        if (id == -1)
            fprintf(stderr, "erreur malloc");
        return 0;
    }
    char *mess = message_server(1, (uint16_t)id, 0, 0);
    if (mess == NULL)
    {
        fprintf(stderr, "erreur malloc");
        return 0;
    }

    if (send(sock, mess, SIZE_MESS_SERV, 0) != SIZE_MESS_SERV)
    {
        perror("send");
        free(mess);
        return 0;
    }
    free(mess);
    return 1;
}

void envoie_message_erreur(int sock)
{
    char *mess = message_server(31, 0, 0, 0);
    if (mess == NULL)
    {
        return;
    }
    send(sock, mess, SIZE_MESS_SERV, 0);
    free(mess);
}

int lire_jusqua_datalen(int sock, uint16_t *numfil, uint16_t *nb, uint8_t *datalen)
{
    buf_t *buf = creer_buf_t(5);
    if (buf == NULL)
        return 0;
    if (!read_msg(sock, buf))
    {
        free_buf(buf);
        return 0;
    }
    *numfil = (ntohs)(((uint16_t *)buf->buf)[0]);
    *nb = (ntohs)(((uint16_t *)buf->buf)[1]);
    *datalen = ((uint8_t *)buf->buf)[4];
    free_buf(buf);
    return 1;
}

int lire_data(int sock, uint8_t datalen, char *data)
{
    buf_t *buf = creer_buf_t(datalen);
    if (!buf)
        return 0;
    if (!read_msg(sock, buf))
    {
        free_buf(buf);
        return 0;
    }
    memmove(data, buf->buf, datalen);
    data[datalen] = '\0';
    free_buf(buf);
    return 1;
}

int confirmer_ajout_billet(int sock, uint16_t numfil, u_int16_t id)
{
    char *mess = message_server(2, id, numfil, 0);
    if (!mess)
        return 0;
    if (send(sock, mess, SIZE_MESS_SERV, 0) != SIZE_MESS_SERV)
    {
        free(mess);
        return 0;
    }
    free(mess);
    return 1;
}

int poster_un_billet(int sock, inscrits_t *inscrits, fils_t *fils, uint16_t id)
{
    char pseudo[LEN_PSEUDO + 1];
    if (!est_inscrit(inscrits, id, pseudo)){ // le client n'est pas inscrit
        return 0;
    }
    u_int16_t numfil;
    u_int16_t nb;
    u_int8_t datalen;
    if (!lire_jusqua_datalen(sock, &numfil, &nb, &datalen))
        return 0;
    if (nb != 0) // nb doit être 0 dans le message client
        return 0;
    if(datalen == 0)
        return 0;
    char data[datalen + 1];
    memset(data, 0, sizeof(data));
    if (!lire_data(sock, datalen, data)){ // on met le texte du billet dans data
        return 0;
    }
    // ajouter le billet
    printf("datalen : %d, data :%s\n", datalen, data);
    if (numfil == 0)
    { // on ajoute le billet dans un nouveau fil
        fil_t *fil = ajouter_nouveau_fil(fils, pseudo);
        if (!fil)
            return 0;
        ajouter_billet(fil, pseudo, datalen, data);
        return confirmer_ajout_billet(sock, fil->num_fil, id);
    }
    // on ajoute le billet dans le fil numfil
    if(!ajouter_billet_num(fils, numfil, pseudo, datalen, data))
        return 0;

    return confirmer_ajout_billet(sock, numfil, id);
}

//réponse à la demande des billets

int annoncer_envoi_billets(int sock, uint16_t numfil, uint16_t nb, uint8_t id){
    char *mess = message_server(3, id, numfil, nb);
    if (!mess)
        return 0;
    if (send(sock, mess, SIZE_MESS_SERV, 0) != SIZE_MESS_SERV)
    {
        free(mess);
        return 0;
    }
    free(mess);
    return 1;
}

uint8_t get_datalen(char* message){
    return (uint8_t)message[2+(2*LEN_PSEUDO)];
}

int envoyer_billets(int sock, char** messages, uint16_t nb_rep){
    for(int i=0; i<nb_rep; i++){
        if(send(sock,messages[i], 23+get_datalen(messages[i]) ,0) != 23+get_datalen(messages[i]))
            return 0;
    }   
    return 1;
}

int demander_des_billets(int sock,inscrits_t *inscrits,fils_t * filst,uint16_t id){
    char pseudo[LEN_PSEUDO + 1];
    if (!est_inscrit(inscrits, id, pseudo)){ // le client n'est pas inscrit
        return 0;
    }
    u_int16_t numfil;
    u_int16_t nb;
    u_int8_t datalen;
    if (!lire_jusqua_datalen(sock, &numfil, &nb, &datalen)){
        return 0;
    }
    if(datalen!=0){
        return 0;
    }

    char** messages = NULL;
    uint16_t numfil_rep;
    uint16_t nb_rep;

    if(!get_messages(filst,numfil,nb, &messages, &numfil_rep, &nb_rep))
        return 0;
    if(!annoncer_envoi_billets(sock,numfil_rep,nb_rep,id)){
        free_messages_billets(messages,nb_rep);
        return 0;
    }
    if(!envoyer_billets(sock,messages,nb_rep)){
        free_messages_billets(messages,nb_rep);
        return 0;
    }
    free_messages_billets(messages,nb_rep);
    return 1;
}