#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>


#include "fichiers.h"
#include "serveur/serveur.h"
#include "serveur/messages_serveur.h"

typedef struct bloc{
    uint16_t num_bloc;
    char* data;
    int data_len;
    struct bloc* next;
}bloc_t;

typedef struct fic{
    int recu_dernier;
    uint16_t id;
    bloc_t * blocs;
    struct fic* next;
}fic_t;

typedef struct temp_fic{
    fic_t * first;
}fics_t;

file_list_t * file_list; 

void free_blocs(bloc_t * b){
    while(b!=NULL){
        bloc_t * tmp = b->next;
        free(b->data);
        tmp = b->next;
        free(b);
        b = tmp;
    }
}


fics_t * init_fics(){
    fics_t* res = (fics_t*) malloc(sizeof(fics_t));
    if(res==NULL){
        perror("malloc");
        return NULL;
    }
    return res;
}

fic_t* add_fic(fics_t * fics, uint16_t id){
    fic_t * fic = (fic_t*) malloc(sizeof(fic_t));
    if(!fic){
        perror("malloc");
        return NULL;
    }
    memset(fic,0,sizeof(fic_t));
    fic->blocs = NULL;
    fic->id = id;

    if(fics->first==NULL){
        fics->first = fic;
        fic->next = NULL;
    }
    else{
        fic->next = fics->first;
        fics->first = fic;
    }
    return fic;
}


/**on suppose que le fichier de id existe*/
void supp_fic(fics_t * fics, uint16_t id){
    fic_t * tmp;
    if(fics->first->id==id){
        tmp = fics->first;
        fics->first = fics->first->next; 
    }
    else{
        fic_t * f = fics->first;
        while(f->next->id!=id)
            f=f->next;
        tmp = f->next;
        f->next = f->next->next;
    }
    free_blocs(tmp->blocs);
    free(tmp);
}

fic_t * get_fic(fics_t * fics, uint16_t id){
    fic_t * f=fics->first;
    while(f!=NULL){
        if(f->id==id)
            return f;
    }
    return NULL;
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

file_list_t * init_file_list(){
    file_list_t * res = (file_list_t*)malloc(sizeof(file_list_t));
    if(!res){
        perror("malloc");
        return res;
    }
    return res;
}

int add_id_file(uint16_t id, char* name_file, uint16_t numfil){
    id_file_t * id_file = (id_file_t*) malloc(sizeof(id_file_t));
    if(!id_file){
        perror("malloc");
        return 0;
    }    
    id_file->id = id;
    id_file->numfil = numfil;
    id_file->name = (char*) malloc(sizeof(char) * (strlen(name_file)+1));
    if(!id_file->name){
        perror("malloc");
        free(id_file);
        return 0;
    }
    memmove(id_file->name, name_file, strlen(name_file));
    id_file->name[strlen(name_file)]='\0';

    id_file->next = file_list->first;
    file_list->first = id_file;
    return 1;
}

void free_id_file(id_file_t * id_file){
    free(id_file->name);
    free(id_file);
}

void rm_id_file(u_int16_t id){
    id_file_t*  tmp;
    if(file_list->first->id==id){
        tmp = file_list->first;
        file_list->first = tmp->next;
        free_id_file(tmp);
    }
    else{
        tmp = file_list->first;
        while(tmp->next->id!=id)
            tmp = tmp->next;
        id_file_t * sav = tmp->next;
        tmp->next=tmp->next->next;
        free_id_file(sav);
    }
}

char * get_name(uint16_t id){
    for(id_file_t * t = file_list->first; t!=NULL ;t=t->next){
        if(t->id==id){
            return t->name;
        }
    }
    return NULL;
}

uint16_t get_numfil(uint16_t id){
    for(id_file_t * t = file_list->first; t!=NULL ;t=t->next){
        if(t->id==id){
            return t->numfil;
        }
    }
    return 0;
}

int recevoir_fichier(int sock, inscrits_t* inscrits, fils_t* filst, uint16_t id,uint16_t port){
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
    if(nb!=0 || datalen==0)
        return 0;
    char file_name[datalen + 1];
    memset(file_name, 0, sizeof(file_name));
    if (!lire_data(sock, datalen, file_name)){ // on met le texte du billet dans data
        return 0;
    }
    // ajouter le billet
    printf("file_name len : %d, file_name :%s\n", datalen, file_name);
    if(!add_id_file(id,file_name,numfil)){
        return 0;    
    }
    if(!annoncer_ecoute_pour_recevoir_fichier(sock,numfil,id,port))
        return 0;
    // on ajoute le fichier dans le fil numfil
    return ajouter_billet_num(filst, numfil, pseudo, datalen, file_name);
}

int save_fic(fic_t * fic){
    char path[50];
    sprintf(path,"serveur/fichiers/fil%d/%s",get_numfil(fic->id),get_name(fic->id));
    int fd = open(path,O_CREAT|O_TRUNC|O_WRONLY,0777);
    if(fd<0){
        perror("open");
        return 0;
    }
    bloc_t * b = fic->blocs;
    int count=0;
    while(b!=NULL){
        int nb = write(fd,b->data,b->data_len);
        if(nb!=b->data_len){
            perror("write");
        }
        b=b->next;
        count++;
    }
    close(fd);
    rm_id_file(fic->id);
    return 1;
}

int add_bloc(fics_t* fics, uint16_t numbloc, uint16_t id, char* data, int len, int count){
    fic_t * fic = get_fic(fics,id);
    if(fic==NULL){//ajouter un fichier
        fic = add_fic(fics,id) ;
        if(fic==NULL)
            return 0;
    }
    
    bloc_t * bloc = (bloc_t*)malloc(sizeof(bloc_t));
    memset(bloc,0,sizeof(bloc_t));
    if(bloc==NULL){
        perror("malloc");
        return 0;
    }
    bloc->num_bloc = numbloc;
    bloc->data_len = len;
    if(len>0){
        bloc->data = (char*)malloc(sizeof(char)*(len+1));
        memmove(bloc->data, data, len);
        bloc->data[len]='\0';
    }    
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
    if(len<LEN_PAQUET){
        fic->recu_dernier = bloc->num_bloc;
    }
    
    if(fic->recu_dernier){//on a eu tout les blocs
        int save = TRUE;
        for(bloc_t* b = fic->blocs; b!=NULL; b=b->next){
            if(b->next!=NULL && b->num_bloc!=b->next->num_bloc-1){
                save=FALSE;
                break;
            }
            if(b->next==NULL && b->num_bloc!=fic->recu_dernier){
                save=FALSE;
                break;
            }
        }
        if(save){
            save_fic(fic);
            supp_fic(fics,id);
        }
    }
    return 1;
}

int transmission_fichiers(int sock_udp){
    fics_t* fics = init_fics();
    int count=0;
    while(1){
        char buff[LEN_PAQUET+5]={0};
        int nb = recvfrom(sock_udp,buff,LEN_PAQUET+5,0,NULL,NULL);
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
        count++;
        add_bloc(fics,num_bloc,id,data, nb-4, count);        
    }
    return 0;
}