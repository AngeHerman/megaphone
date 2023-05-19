#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "abonnement.h"
#include "reponses_serveur.h"
#include "../lecture.h"

#define NB_OCTETS_NOTIF 34
pthread_mutex_t verrou_affichage = PTHREAD_MUTEX_INITIALIZER;

void * abonnement(void * args){
    info_abonn infos = *((info_abonn *)args);

    /* créer la socket */
    int sock;
    if((sock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        perror("echec de socket");
        return NULL;
    }

    /* SO_REUSEADDR permet d'avoir plusieurs instances locales de cette application  */
    /* ecoutant sur le port multicast et recevant chacune les differents paquets     */
    int ok = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &ok, sizeof(ok)) < 0) {
        perror("echec de SO_REUSEADDR");
        close(sock);
        return NULL;
    }

    /* Initialisation de l'adresse de reception */
    struct sockaddr_in6 grsock;
    memset(&grsock, 0, sizeof(grsock));
    grsock.sin6_family = AF_INET6;
    grsock.sin6_addr = in6addr_any;
    grsock.sin6_port = infos.port;
  
    if(bind(sock, (struct sockaddr*) &grsock, sizeof(grsock))) {
        perror("echec de bind");
        close(sock);
        return NULL;
    }
  
    /* s'abonner au groupe multicast */
    struct ipv6_mreq group;
    group.ipv6mr_multiaddr = infos.addr_diffus;
    group.ipv6mr_interface = 0;

    if(setsockopt(sock, IPPROTO_IPV6, IPV6_JOIN_GROUP, &group, sizeof group) < 0) {
        perror("echec de abonnement groupe");
        close(sock);
        return NULL;
    }

    /*recevoir les messages*/
    while(1){
        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(sock, &rset); //pour surveillance en lecture de sock
        select(sock+1, &rset, NULL, 0, NULL);
        if (FD_ISSET(sock, &rset)) {
            char mess_notif[NB_OCTETS_NOTIF] = {0};
            buf_t *buf = creer_buf_t(NB_OCTETS_NOTIF);
            if (buf == NULL){
                perror("erreur creation buf");
                continue;
            }
            int r =read_msg(sock, buf);
            if (r == 0){
                perror("erreur de read");
                free(buf);
                continue;
            }
            memmove(mess_notif, buf->buf, NB_OCTETS_NOTIF);
            free_buf(buf);
            //récupérer les informations
            uint16_t numfil;
            char pseudo[10+1] = {0};
            char data[20+1] = {0};
            if(!notification(mess_notif, &numfil,pseudo, data)){
                continue;
            }
            pthread_mutex_lock(&verrou_affichage);
            printf("Notification -> nouveau billet sur le fil %d :\n", numfil);
            printf("pseudo du billet : %s\n", pseudo);
            printf("les 20 premiers octets du billet : %s\n", data);
            pthread_mutex_unlock(&verrou_affichage);
        }
    }
    close(sock);
    return NULL;
}