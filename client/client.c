#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>

#include "client.h"
#include "messages_client.h"
#include "../lecture.h"
#include "reponses_serveur.h"




void affiche_adresse(struct sockaddr_in6 *adr){
    char adr_buf[INET6_ADDRSTRLEN];
    memset(adr_buf, 0, sizeof(adr_buf));

    inet_ntop(AF_INET6, &(adr->sin6_addr), adr_buf, sizeof(adr_buf));
    printf("adresse serveur : IP: %s port: %d\n", adr_buf, ntohs(adr->sin6_port));
}

int get_server_addr(char* hostname, char* port, int * sock, struct sockaddr_in6** addr, int* addrlen) {
    struct addrinfo hints, *r, *p;
    int ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_V4MAPPED | AI_ALL;

    if ((ret = getaddrinfo(hostname, port, &hints, &r)) != 0 || NULL == r){
        fprintf(stderr, "erreur getaddrinfo : %s\n", gai_strerror(ret));
        return -1;
    }
  
    *addrlen = sizeof(struct sockaddr_in6);
    p = r;
    while( p != NULL ){
        if((*sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) > 0){
            if(connect(*sock, p->ai_addr, *addrlen) == 0)
                break;
            close(*sock);
        }

        p = p->ai_next;
    }

    if (NULL == p) return -2;
    *addr = (struct sockaddr_in6 *) p->ai_addr;
    freeaddrinfo(r);
    return 0;
}

int get_server_addr_envoi_fichier(char* hostname, char* port, int * sock, struct sockaddr_in6** addr, int* addrlen){
    struct addrinfo hints, *r, *p;
    int res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    if ((res = getaddrinfo(hostname, port, &hints, &r)) != 0 || NULL == r){
        fprintf(stderr, "echec getaddrinfo : %s\n", gai_strerror(res));
        return 0;
    }   
    else{
        p = r;
        while( p != NULL ){
            if((*sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) > 0)
                break;
            close(*sock);
            p = p->ai_next;
        }
    }
    if(p == NULL)
        return 0;
    *addr = (struct sockaddr_in6 *) p->ai_addr;
    *addrlen = sizeof(struct sockaddr_in6);
    freeaddrinfo(r);
    return 1;

}

int get_data(char *data,int taille,int sock){
    buf_t *buf = creer_buf_t(taille);
    if (buf == NULL){
        perror("erreur creation buf");
        return 0;
    }
    int r =read_msg(sock, buf);
    if (r == 0){
        perror("erreur de read");
        free(buf);
        return 0;
    }
    memmove(data, buf->buf, taille);
    free_buf(buf);
    return 1;
}


int demande_dernier_billets(int sock,u_int16_t id_client,uint16_t numfil, uint16_t nb){

    char * mess_dernier_billets = message_dernier_billets(id_client,numfil,nb);
    if(send(sock, mess_dernier_billets,LEN_MESS_CLIENT,0) != LEN_MESS_CLIENT){
        free(mess_dernier_billets);
        return 0;
    } 
    free(mess_dernier_billets);

    //réponse du serveur
    u_int16_t rep[3];
    int taille = 0;
    if((taille = recv(sock,rep, sizeof(rep) ,0)) != sizeof(rep)){
        printf("Taille recv est %d\n",taille);
        return 0;
    }

    uint16_t nbb = reponse_derniers_billets(rep);
    printf("Il ya exactement %u messages qui arrivent\n",nbb);

    for(int i = 0; i < nbb; i++){
        char data[NB_OCTECS_DERNIERS_MESSAGE_JUSQUA_DATALEN];
        memset(data,0,NB_OCTECS_DERNIERS_MESSAGE_JUSQUA_DATALEN);
        if (!get_data(data,NB_OCTECS_DERNIERS_MESSAGE_JUSQUA_DATALEN,sock))
            return 0;
        uint8_t datalen = reponse_derniers_billets_datalen(data);

        char message[datalen+1];
        memset(message,0,datalen+1);
        if (!get_data(message,datalen,sock))
            return 0;
        printf("Message %d : %s\n",i+1,message);
        printf("-------------------------------\n");
    }
    return 1;
}

int demande_abonnement(int sock,u_int16_t id_client,uint16_t numfil){
    char * mess_abonnement_fil = message_abonnement_fil(id_client,numfil);
    if(send(sock, mess_abonnement_fil,LEN_MESS_CLIENT,0) != LEN_MESS_CLIENT){
        free(mess_abonnement_fil);
        return 0;
    } 
    free(mess_abonnement_fil);
    char data[NB_OCTECS_REPONSES_ABONNEMENT];
    memset(data,0,NB_OCTECS_REPONSES_ABONNEMENT);
    if (!get_data(data,NB_OCTECS_REPONSES_ABONNEMENT,sock))
        return 0;
    char addr[17];
    memset(addr,0,17);
    uint16_t *port;
    if(!reponse_abonnement(data,addr,port))
        return 0;
    printf("port est %d\n",*port);
    printf("addr est %s\n",addr);
    return 1;
}


uint16_t demande_inscription(int fd_sock, char * pseudo){
    char * mess_inscr = message_inscription_client(pseudo);

    if(send(fd_sock, mess_inscr,LEN_MESS_INSCR,0) != LEN_MESS_INSCR){
        return 0;
    } 
    free(mess_inscr);

    //réponse du serveur
    u_int16_t rep[3];
    int len = 0;
    if( (len = recv(fd_sock,rep, sizeof(rep) ,0)) != sizeof(rep)){
        printf("%d\n", len);
        return 0;
    }
    uint16_t id = reponse_inscription(rep);
    return id;
}



uint16_t poster_un_billet(int sock,uint16_t id, uint16_t num_fil, uint8_t datalen, char * data){
    char * mess = message_client(CODE_REQ_P0STER_BILLET,id,num_fil,0,datalen,data);

    if(send(sock,mess,7+datalen,0)!=7+datalen){
        free(mess);
        return 0;
    }
    free(mess);

    //réponse du serveur
    u_int16_t rep[3];
    int len = 0;
    if( (len = recv(sock,rep, sizeof(rep) ,0)) != sizeof(rep)){
        printf("%d\n", len);
        return 0;
    }
    return reponse_poster_billet(rep);
}

long int taille_fichier(char *file_name)
{
    int fd = open(file_name, O_RDONLY);
  
    if (fd == -1) {
        perror("erreur open");
        return 0;
    }

    long int size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        perror("lseek");
        close(fd);
        return 0;
    }
    close(fd);
    return size;
}

int envoi_par_paquets_de_512(int fd, int sock,int id,int taille_fic){
    int nb_paquets = taille_fic/TAILLE_PAQUET_UDP;
    //On lit et on envoie par 512. Si le fichier est < 512 octets on entre pas dans la boucle
    for(int i = 0; i <= nb_paquets; i++){
        int taille_a_lire = TAILLE_PAQUET_UDP;
        //Le cas où on doit envoyer le dernier message < 512 octets
        if(i == nb_paquets){
            taille_a_lire = taille_fic%TAILLE_PAQUET_UDP; //La taille du dernier paquet   soit 0 soit < 512
        }
        char data[taille_a_lire];
        memset(data,0,taille_a_lire);
        if(read(fd, data, taille_a_lire) != taille_a_lire){
            perror("read");
            return 0;
        }
        char * mess_paquet_udp = message_client_udp(CODE_REQ_AJOUT_FICHIER,id,i+1,taille_a_lire,data);

        if(sendto(sock, mess_paquet_udp, 4 + taille_a_lire, 0, NULL, 0) != 4 + taille_a_lire){
            free(mess_paquet_udp);
            return 0;
        }
        free(mess_paquet_udp);

    }
    return 1;

}

int envoi_fichier(uint16_t id, uint16_t port,char * nom_fichier,char * hostname){
    struct sockaddr_in6* server_addr;
    int sockUDP, adrlen;
    char port_en_char[10];
    memset(port_en_char,0,10);
    sprintf(port_en_char, "%u", port);
    if(!get_server_addr(hostname,port_en_char, &sockUDP, &server_addr, &adrlen))
        return 0;
    char chemin [TAILLE_MAX_STRING];
    memset(chemin,0,TAILLE_MAX_STRING);
    strcpy(chemin,CHEMIN_FICHIER_CLIENT);
    strcat(chemin,nom_fichier);
    int fd= open(chemin, O_RDONLY);
    if (fd == -1) {
        perror("erreur open");
        return 0;
    }
    long int taille_fic = taille_fichier(chemin);
    if(taille_fic == 0)
        return 0;
    if(taille_fic >= TAILLE_MAX_AJOUT_FICHIER){
        printf("Fichier trop gros, taille max est %d\n",TAILLE_MAX_AJOUT_FICHIER);
        return 0;
    }
    if(!envoi_par_paquets_de_512(fd,sockUDP,id,taille_fic))
        return 0;
    return 1;
    
}

uint16_t ajouter_un_fichier(int sock, uint16_t id, uint16_t num_fil, uint8_t taille_nom_fichier, char * nom_fichier,char * hostname){
    char * mess_demande_ajout_fichier = message_client(CODE_REQ_AJOUT_FICHIER,id,num_fil,0,taille_nom_fichier,nom_fichier);
    if(send(sock,mess_demande_ajout_fichier,LEN_MESS_CLIENT+taille_nom_fichier,0) != LEN_MESS_CLIENT+taille_nom_fichier){
        free(mess_demande_ajout_fichier);
        return 0;
    }
    free(mess_demande_ajout_fichier);
    char data[NB_OCTECS_MESSAGE_SERVEUR];
    memset(data,0,NB_OCTECS_MESSAGE_SERVEUR);
    if (!get_data(data,NB_OCTECS_MESSAGE_SERVEUR,sock))
        return 0;
    u_int16_t port = reponse_ajout_fichier(data);
    if(!port)
        return 0;
    close(sock);
    if(!envoi_fichier(id,port,nom_fichier,hostname))
        return 0;

}
