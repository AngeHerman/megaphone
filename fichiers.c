#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "fichiers.h"
#include "serveur/serveur.h"
#include "serveur/messages_serveur.h"

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

/**
 * @brief renvoie un buffer pour stocker les blocs reçus du fichier
 * 
 * @param id identifiant du client qui envoie le fichier
 * @param namefile le nom du fichier
 * @param numfil le fil sur lequel le fichier a été ajouté
 * @return le buffer pour stocker les blocs du fichier, ou NULL en cas d'erreur
 */
fic_t * init_fic(uint16_t id, char * namefile, uint16_t numfil){
    fic_t * fic = (fic_t*)malloc(sizeof(fic_t));
    if(!fic)
        return NULL;
    fic->num_dernier_bloc = 0;
    fic->nb_bloc_recus = 0;
    fic->id = id;
    fic->namefile = (char*)malloc(strlen(namefile)+1);
    memmove(fic->namefile, namefile, strlen(namefile));
    fic->namefile[strlen(namefile)] = '\0';
    fic->numfil = numfil;
    fic->blocs = NULL;
    return fic;
}

/**
 * @brief libère la mémoire allouée pour la liste chaînée des blocs du fichier
 * 
 * @param b pointeur vers le premier maillon de la liste
 */
void free_blocs(bloc_t * b){
    while(b!=NULL){
        bloc_t * tmp = b->next;
        if(b->data!=NULL)
            free(b->data);
        tmp = b->next;
        free(b);
        b = tmp;
    }
}

/**
 * @brief libère la mémoire alloué pour le buffer du fichier
 * 
 * @param fic pointeur vers le buffer du fichier 
 */
void free_fic(fic_t * fic){
    free(fic->namefile);
    free_blocs(fic->blocs);
    free(fic);    
}

/**
 * @brief ecrit le fichier dans la mémoire (dans le repertoire serveur/fichiers/fil`numfil`)
 * 
 * @param fic buffer contenant les données du fichier
 * @return int 1 en cas de succes, 0 en cas d'erreur
 */
int save_fic(fic_t * fic){
    char path[50];
    sprintf(path,"serveur/fichiers/fil%d/%s",fic->numfil,fic->namefile);
    int fd = open(path,O_CREAT|O_TRUNC|O_WRONLY,0777);
    if(fd<0){
        perror("open");
        return 0;
    }
    bloc_t * b = fic->blocs;
    while(b!=NULL){
        int nb = write(fd,b->data,b->data_len);
        if(nb!=b->data_len){
            perror("write");
        }
        b=b->next;
    }
    close(fd);
    return 1;
}

/**
 * @brief ajoute un bloc contenant `data` dans la liste des blocs avant le bloc de numéro
 * strictement supérieur que `numbloc`
 * si tous les blocs constituant le fichier ont été recus, écrit le fichier dans la mémoire
 * 
 * @param fic le buffer du fichier
 * @param numbloc le numéro du bloc
 * @param data les données
 * @param len la taille de data
 * @return int FALSE si on a recu tout le fichier, TRUE sinon
 */
int add_bloc(fic_t* fic, uint16_t numbloc,char* data, int len){
    bloc_t * bloc = (bloc_t*)malloc(sizeof(bloc_t));
    memset(bloc,0,sizeof(bloc_t));
    if(bloc==NULL){
        perror("malloc");
        return TRUE;
    }
    bloc->num_bloc = numbloc;
    bloc->data_len = len;
    if(len>0){
        bloc->data = (char*)malloc(sizeof(char)*(len+1));
        memmove(bloc->data, data, len);
        bloc->data[len]='\0';
    }   
    //ajout du bloc dans la liste avant le premier bloc de numéro strictement superieur
    if(fic->blocs==NULL){
        fic->blocs = bloc;
        bloc->next = NULL;
    }
    else{
        bloc_t * b = fic->blocs;
        if(fic->blocs->num_bloc>bloc->num_bloc){
            bloc->next=fic->blocs;
            fic->blocs=bloc;
        }
        else{
            while(b->next!=NULL && b->next->num_bloc<bloc->num_bloc)
                b=b->next;
            bloc->next = b->next;
            b->next = bloc;
        }
    }
    fic->nb_bloc_recus++;
    if(len<LEN_PAQUET){
        fic->num_dernier_bloc = bloc->num_bloc;
        printf("num dernier paquet reçu %d\n", bloc->num_bloc);
    }
    
    if(fic->num_dernier_bloc == fic->nb_bloc_recus){//on a eu tout les blocs
        save_fic(fic);
        return FALSE;
    }
    return TRUE;
}

int transmission_fichiers(int sock_udp, char* name_file, uint16_t id, uint16_t numfil){
    char buff[LEN_PAQUET+5]={0};
    int nb=0;//la taille du dernier paquet reçu
    int taille_fic=0;//taille du fichier reçu
    int continuer = TRUE;//on n'a pas encore reçu tous les paquets
    fic_t * fic;
    if(!(fic = init_fic(id, name_file, numfil)))
        return -1;
    while(continuer){
        nb=recvfrom(sock_udp,buff,LEN_PAQUET+5,0,NULL,NULL);
        if(nb==-1){
            perror("recvfrom");
            continue;
        }
        buff[nb]='\0';
        uint16_t entete = ntohs(*(uint16_t*)buff);
        uint16_t num_bloc = ntohs(*(((uint16_t*)buff)+1));
        char * data = buff+4;
        uint8_t code_req = get_code_req(entete);
        uint16_t id = get_id_requete(entete);
        if(code_req!=5){
            perror("codereq error");
            continue;
        }
        taille_fic+=nb-4;
        continuer = add_bloc(fic,num_bloc,data, nb-4);      
    }
    return taille_fic;
}