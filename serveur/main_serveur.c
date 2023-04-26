#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


#include "inscrits.h"
#include "fils.h"
#include "serveur.h"
#include "messages_serveur.h"
#include "../lecture.h"
#include "../fichiers.h"

extern file_list_t * file_list; 
inscrits_t * inscrits;
fils_t * fils;
pthread_mutex_t verrou_inscription = PTHREAD_MUTEX_INITIALIZER;
uint16_t port_udp;

void *serve(void *arg) {
    int sock = *((int *) arg);
    u_int16_t entete =lire_entete(sock);
    if(entete == 0){
        perror("erreur réception du message");
        return NULL;
    }

    int *ret = (int *)malloc(sizeof(int));
    int rep=0;
    /*recupération du codereq et verification du type de demande*/
    switch(get_code_req(entete)){
        /*demande d'inscription*/
        case 1:
            pthread_mutex_lock(&verrou_inscription); 
            rep = inscrire_client(sock, inscrits);
            pthread_mutex_unlock(&verrou_inscription);
            break;
        /*poster un billet*/
        case 2:
            rep = poster_un_billet(sock,inscrits,fils, get_id_requete(entete));
            break;
        /*derniers n billets d'un fil*/
        case 3:
            rep = demander_des_billets(sock,inscrits,fils,get_id_requete(entete));
            break;
        /*recevoir un fichier*/
        case 5:
            rep = recevoir_fichier(sock,inscrits,fils,get_id_requete(entete),port_udp);
    }
    if(rep){//succès
        *ret = 1;
        close(sock);
        pthread_exit(ret);
    }
    //envoyer le message d'erreur
    envoie_message_erreur(sock);
    *ret=0;
    close(sock);
    pthread_exit(ret);
}

void * serve_udp(void* arg){
    int * ret = (int*) malloc(sizeof(int));
    * ret = 0;
    int sock_udp = socket(PF_INET6, SOCK_DGRAM, 0);
    if (sock_udp < 0) {
        *ret = -1;
        return ret;
    }
    struct sockaddr_in6 servadrudp;
    memset(&servadrudp, 0, sizeof(servadrudp));
    servadrudp.sin6_family = AF_INET6;
    servadrudp.sin6_addr = in6addr_any;
    servadrudp.sin6_port = htons(port_udp);
    if (bind(sock_udp, (struct sockaddr *)&servadrudp, sizeof(servadrudp)) < 0) {
        *ret = -1;
        return ret;
    }
    *ret = transmission_fichiers(sock_udp);
    close(sock_udp);
    return ret;
}

int main(int argc, char *argv[]){

    if(argc<3){
        fprintf(stderr, "Usage : %s <num_port> <num_pord_udp>\n", argv[0]);
        exit(1);
    }

    /*Creation liste d'inscrits*/
    inscrits = creer_inscrits_t();
    /*Création liste de fils*/
    fils = creer_list_fils();
    file_list = init_file_list();

    //*** creation de l'adresse du destinataire (serveur) ***
    struct sockaddr_in6 address_sock;
    memset(&address_sock, 0, sizeof(address_sock));
    address_sock.sin6_family = AF_INET6;
    address_sock.sin6_port = htons(atoi(argv[1]));
    address_sock.sin6_addr = in6addr_any;

    //*** creation de la socket ***
    int sock = socket(PF_INET6, SOCK_STREAM, 0);
    if(sock < 0){
        perror("creation socket");
        exit(1);
    }
    int optval = 0;
    int r = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &optval, sizeof(optval));
    if (r < 0) 
        perror("erreur connexion IPv4 impossible");

    //*** le numero de port peut etre utilise en parallele ***
    optval = 1;
    r = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (r < 0) 
        perror("erreur réutilisation de port impossible");

    //*** on lie la socket au port ***
    r = bind(sock, (struct sockaddr *) &address_sock, sizeof(address_sock));
    if (r < 0) {
        perror("erreur bind");
        exit(2);
    }

    //*** Le serveur est pret a ecouter les connexions sur le port ***
    r = listen(sock, 0);
    if (r < 0) {
        perror("erreur listen");
        exit(2);
    }

    port_udp = atoi(argv[2]);
    pthread_t thread_udp;
    if(pthread_create(&thread_udp,NULL,serve_udp,NULL)==-1){
        perror("pthread_create");
        exit(3);
    }
    
    while(1){
        struct sockaddr_in6 addrclient;
        socklen_t size=sizeof(addrclient);
        
        //*** on crée la varaiable sur le tas ***
        int *sock_client = (int*)malloc(sizeof(int));

        //*** le serveur accepte une connexion et initialise la socket de communication avec le client ***
        *sock_client = accept(sock, (struct sockaddr *) &addrclient, &size);

        if (*sock_client >= 0) {
            pthread_t thread;
            //*** le serveur cree un thread et passe un pointeur sur socket client à la fonction serve ***
            if (pthread_create(&thread, NULL, serve, sock_client) == -1) {
                perror("pthread_create");
                continue;
            }  
            //*** affichage de l'adresse du client ***
            char nom_dst[INET6_ADDRSTRLEN];
            printf("client connecte : %s %d\n", inet_ntop(AF_INET6,&addrclient.sin6_addr,nom_dst,sizeof(nom_dst)), htons(addrclient.sin6_port));
        }
    }
    //*** fermeture socket serveur ***
    close(sock);
    return 0;
}  
