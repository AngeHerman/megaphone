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
#include "../fichiers.h"

#define NB_OCTECS_REPONSES_ABONNEMENT 22//code_req et ID (2 otects)+Numfil (2 otects)+nb (2) + adresse(16)

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
    printf("client %s inscrit avec id %d\n", pseudo,id);
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
    if (nb != 0 || datalen==0) // nb doit être 0 dans le message client
        return 0;
    char data[datalen + 1];
    memset(data, 0, sizeof(data));
    if (!lire_data(sock, datalen, data)){ // on met le texte du billet dans data
        return 0;
    }
    // ajouter le billet
    printf("datalen : %d, data :%s\n", datalen, data);
    if (numfil == 0){ // on ajoute le billet dans un nouveau fil
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
    /*On fait une copie de la liste des fils pour éviter des modification par un autre
    thread entre le moment où on compte le nombre de messages à envoyer et où on récupère les messages*/
    fils_t* copy_fils = copy_list_fils(filst);
    if(!get_messages(copy_fils,numfil,nb, &messages, &numfil_rep, &nb_rep)){
        free_fils(copy_fils);
        return 0;
    }
    free_fils(copy_fils);
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

int creer_socket_udp(int* sock_udp, int* port){
    *sock_udp = socket(PF_INET6, SOCK_DGRAM, 0);
    if (*sock_udp < 0) {
        return 0;
    }
    struct sockaddr_in6 servadrudp;
    memset(&servadrudp, 0, sizeof(servadrudp));
    servadrudp.sin6_family = AF_INET6;
    servadrudp.sin6_addr = in6addr_any;
    //le système choisira un port
    servadrudp.sin6_port = htons(0);
    if (bind(*sock_udp, (struct sockaddr *)&servadrudp, sizeof(servadrudp)) < 0) {
        return 0;
    }
    //obtenir le port choisi
    socklen_t addrlen = sizeof(servadrudp);
    if(getsockname(*sock_udp,(struct sockaddr*)&servadrudp,&addrlen)==-1){
        return 0;
    }
    *port = ntohs(servadrudp.sin6_port);
    return 1;
}

int annoncer_ecoute_pour_recevoir_fichier(int sock, uint16_t numfil, uint8_t id, uint16_t port){
    char *mess = message_server(5, id, numfil, port);
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

int recevoir_fichier(int* sock, inscrits_t* inscrits, fils_t* filst, uint16_t id){
    char pseudo[LEN_PSEUDO + 1];
    if (!est_inscrit(inscrits, id, pseudo)){ // le client n'est pas inscrit
        return 0;
    }
    u_int16_t numfil;
    u_int16_t nb;
    u_int8_t datalen;
    if (!lire_jusqua_datalen(*sock, &numfil, &nb, &datalen)){
        return 0;
    }
    if(nb!=0 || datalen==0)
        return 0;
    char* file_name= (char*)malloc(sizeof(char)*(datalen+1));
    memset(file_name, 0, datalen+1);
    if (!lire_data(*sock, datalen, file_name)){ // on met le texte du billet dans data
        return 0;
    }
    // ajouter le billet
    printf("file_name len : %d, file_name :%s\n", datalen, file_name);

    int port_udp;
    int * sock_udp = (int*)malloc(sizeof(int));
    if(!sock_udp){
        perror("malloc");
        return 0;
    }
    if(!creer_socket_udp(sock_udp,&port_udp))
        return 0;
    
    if(!annoncer_ecoute_pour_recevoir_fichier(*sock,numfil,id,port_udp))
        return 0;
    //terminer la connexion
    close(*sock);
    *sock = -1;
    
    int file_len = reception_par_paquets_de_512(*sock_udp, file_name, id, numfil,0);

    char* billet_file=(char*)malloc(datalen+100);
    if(!billet_file){
        perror("malloc");
        return 0;
    }
    memset(billet_file,0,datalen+100);
    sprintf(billet_file,"fichier : %s, taille : %d", file_name, file_len);
    free(file_name);
    int ret = ajouter_billet_num(filst, numfil, pseudo, strlen(billet_file), billet_file);
    free(billet_file);
    return ret;
}

int confirmer_abonnement(int sock, fils_t* filst,uint16_t numfil, u_int16_t id){
    struct sockaddr_in6 addr_multi;
    int r = abonner_fil(filst, numfil, &addr_multi);
    if(r==0)
        return 0;
    char *mess = message_confirmer_abonnement(id, numfil, addr_multi);
    if (!mess)
        return 0;
    if (send(sock, mess, NB_OCTECS_REPONSES_ABONNEMENT, 0) != NB_OCTECS_REPONSES_ABONNEMENT)
    {
        free(mess);
        return 0;
    }
    free(mess);
    if(r==2){//on lance la multidifusion
        info_multi* infos = malloc(sizeof(info_multi));
        if(!infos){
            perror("malloc");
            return 0;
        }
        infos->fils = filst;
        infos->numfil = numfil;
        infos->addr_multi = addr_multi;
        pthread_t thread;
        if (pthread_create(&thread, NULL, multi_diffusion, infos) == -1) {
            perror("pthread_create");
            free(infos);
            return 0;
        }     
    }
    return 1;
}

int abonner_a_fil(int sock, inscrits_t* inscrits, fils_t* filst, uint16_t id){
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
    
    int r = confirmer_abonnement(sock, filst, numfil, id);
    return r;
}