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
    ajouter_billet_num(fils, numfil, pseudo, datalen, data);

    return confirmer_ajout_billet(sock, numfil, id);
}

int demander_des_billets(int sock,inscrits_t *inscrits,fils_t * filst,uint16_t id){
    char pseudo[LEN_PSEUDO + 1];
    if (!est_inscrit(inscrits, id, pseudo)){ // le client n'est pas inscrit
        return 0;
    }
    u_int16_t numfil;
    u_int16_t nb;
    u_int8_t datalen;
    u_int16_t numfilRet;
    u_int16_t nbRet = 0;
    int allFils = 0;
    if (!lire_jusqua_datalen(sock, &numfil, &nb, &datalen)){
        return 0;
    }
    if(datalen!=0){
        return 0;
    }
    if(numfil==0){
        allFils= 1;
        numfilRet=filst->nb_fils;
        for(int i= 0;i<filst->nb_fils;i=i+1){
            if(nb>((filst->fils+numfil-1)->nb_billets) || nb ==0){
                nbRet = nbRet+((filst->fils+numfil-1)->nb_billets);
            }
            else {
                nbRet=nbRet+nb;
            }
        }
    }
    else if(numfil>0){
        numfilRet = numfil;
        printf("numfil %u nbbillets %u\n",(filst->fils+numfil)->num_fil,(filst->fils+numfil)->nb_billets);
        if(((filst->fils+numfil-1)->nb_billets)>0){
            if(nb>((filst->fils+numfil-1)->nb_billets) || nb ==0){
                nbRet = (filst->fils+numfil-1)->nb_billets;
            }
            else {
                nbRet = nb;
            }
        }
        else {
            printf("ICI mais nb est %d\n",nb);
            nbRet = 0;
        }
    }
    printf("-----------numfilret est %u\n",numfilRet);
    char *mess = message_server(3, id, numfilRet, nbRet);
    if (!mess){
        return 0;
    }
    if (send(sock, mess, SIZE_MESS_SERV, 0) != SIZE_MESS_SERV){
        perror("send");
        free(mess);
        return 0;
    }
    free(mess);
    if(allFils == 1){
        for(int filAct=1;filAct<=numfilRet;filAct=filAct+1){
            for(int i=(filst->fils+filAct-1)->nb_billets;i>0 && i>((filst->fils+filAct-1)->nb_billets)-nbRet;i=i-1){
                billet_t * billet_tmp = (filst->fils+filAct-1)->billets+i;
                char * res = (char *)malloc(23+(billet_tmp->data_len * sizeof(char)));
                if(res==NULL){
                    perror("malloc");
                    return 0;
                }
                ((uint16_t *)res)[0] = htons(filAct);
                unsigned int len_origine = strlen((filst->fils+filAct-1)->origine);
                memmove(res+2,(filst->fils+filAct-1)->origine,len_origine);
                unsigned int len_pseudo = strlen(billet_tmp->pseudo);
                memmove(res+12, billet_tmp->pseudo, len_pseudo);    
                ((u_int8_t *)res)[22] = billet_tmp->data_len;
                //copier le texte du message
                if(datalen > 0){
                    memmove(res+23, billet_tmp->data, billet_tmp->data_len);    
                }
                if (send(sock, res, sizeof(res), 0) != SIZE_MESS_SERV){
                    perror("send");
                    free(mess);
                    return 0;
                }
                free(res);
            }
        }
    }
    else {
        for(int i=(filst->fils+numfilRet-1)->nb_billets;i>0 && i>((filst->fils+numfilRet-1)->nb_billets)-nbRet;i=i-1){
            billet_t * billet_tmp = (filst->fils+numfilRet-1)->billets+i;
            char * res = (char *)malloc(23+(billet_tmp->data_len * sizeof(char)));
            if(res==NULL){
                perror("malloc");
                return 0;
            }
            ((uint16_t *)res)[0] = htons(numfilRet);
            unsigned int len_origine = strlen((filst->fils+numfilRet-1)->origine);
            memmove(res+2,(filst->fils+numfilRet-1)->origine,len_origine);
            unsigned int len_pseudo = strlen(billet_tmp->pseudo);
            memmove(res+12, billet_tmp->pseudo, len_pseudo);    
            ((u_int8_t *)res)[22] = billet_tmp->data_len;
            //copier le texte du message
            if(datalen > 0){
                memmove(res+23, billet_tmp->data, billet_tmp->data_len);    
            }
            if (send(sock, res, sizeof(res), 0) != SIZE_MESS_SERV){
                perror("send");
                free(mess);
                return 0;
            }
            free(res);
        }
    }
    return 1;
}