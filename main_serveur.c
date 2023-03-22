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

inscrits_t * inscrits;
fils_t * fils;

void *serve(void *arg) {
    int sock = *((int *) arg);
    /*reception de message*/
    uint16_t buf;
    int r =recv(sock,&buf, sizeof(buf) ,0);
    if(r == sizeof(buf)) {
        /*recupération du codereq et verification du type de demande*/
        switch(codereq_recu(buf)){
            /*demande d'inscription*/
            case 1: 
                /*valeurs du message retour*/
                char buf[10+1];
                memset(buf, 0, sizeof(buf));
                if(recv(sock,&buf, sizeof(buf) ,0) != sizeof(buf)){
                    perror("recv");
                }
                buf[10]= '\0';
                uint16_t id = add_user(inscrits,buf);
                uint16_t code_req = 1;
                uint16_t numfil = 0;
                uint16_t nb = 0;
                /*renvoie message inscription*/
                char * res = message_inscription_server(code_req, id, numfil, nb);
                if(send(sock,res,sizeof(res),0) != sizeof(res)){
                    perror("send");
                    close(sock);
                    int *ret = malloc(sizeof(int));
                    *ret = 1;
                    pthread_exit(ret);
                }
                break;
        }
        close(sock);
        return NULL;
    }
}

int main(int argc, char *argv[]){
    /*Creation liste d'inscrits*/
    inscrits = creer_inscrits_t();
    fils = creer_fils_t();

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
    
    while(1){
        struct sockaddr_in6 addrclient;
        socklen_t size=sizeof(addrclient);
        
        //*** on crée la varaiable sur le tas ***
        int *sock_client = malloc(sizeof(int));

        //*** le serveur accepte une connexion et initialise la socket de communication avec le client ***
        *sock_client = accept(sock, (struct sockaddr *) &addrclient, &size);

        if (sock_client >= 0) {
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
