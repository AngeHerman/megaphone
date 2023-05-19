#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "client.h"
#include "interaction.h"

#define DEFAULT_SERVER "lulu"
#define DEFAULT_PORT "7777"
#define SIZE_MESS 100


int main(int argc, char** argv) {
    char hostname[SIZE_MESS];
    char port[SIZE_MESS];
    if (argc < 3) {
        sprintf(hostname,"%s",DEFAULT_SERVER);
        sprintf(port,"%s",DEFAULT_PORT);
    }else{
        sprintf(hostname,"%s",argv[1]);
        sprintf(port,"%s",argv[2]);
    }
    struct sockaddr_in6 server_addr;
    int fdsock, adrlen;

    switch (get_server_addr(hostname,port, &fdsock, &server_addr, &adrlen)) {
    case 0: 
        printf("adresse creee !\n"); 
        break;
    case -1:
        fprintf(stderr, "Erreur: hote non trouve.\n"); 
        exit(1);
    case -2:
        fprintf(stderr, "Erreur: echec de creation de la socket.\n");
        exit(1);
    }

    int ret;
    while(1){
        if(fdsock==-1){
            if((fdsock = socket(PF_INET6, SOCK_STREAM, 0)) > 0){
                if(connect(fdsock,(struct sockaddr *) &server_addr, sizeof(server_addr)) ==-1 )
                    break;
            }
            else
                break;
        }
        //demander au client l'action voulue et l'exécuter
        ret = choix_client(fdsock,hostname);
        
        close(fdsock);
        fdsock = -1;
    }
    return ret;
}