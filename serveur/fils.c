#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#include "fils.h"
#include "messages_serveur.h"

#define CAP_FILS_INIT 10
#define CAP_FIL_INIT 10
#define LEN_PSEUDO 10
#define FALSE 0
#define TRUE 1


fils_t * creer_list_fils(){
    fils_t * res = (fils_t*) malloc(sizeof(fils_t));
    if(res == NULL){
        perror("malloc");
        return NULL;
    }
    memset(res,0,sizeof(fils_t));
    res->capacite = CAP_FILS_INIT;
    res->fils = (fil_t *)malloc(CAP_FILS_INIT * sizeof(fil_t));
    if(res->fils == NULL){
        perror("malloc");
        free(res);
        return NULL;
    }
    return res;
}

void free_fils(fils_t * fs){
    for(int i=0; i<fs->nb_fils; i++){
        free_fil(fs->fils[i]);
    }
    free(fs);
}

void free_fil(fil_t fil){
    for(int i=0; i<fil.nb_billets; i++){
        free(fil.billets[i].data);
    }
    free(fil.billets);
}

fil_t * ajouter_nouveau_fil(fils_t * fs, char * orig){
    if(fs->nb_fils == 65535){//il n'y a plus de place
        fprintf(stderr ,"nombre max de fils atteint");
        return NULL;
    }
    //vérifier s'il la capacité est insuffisante
    if(fs->capacite == fs->nb_fils){
        fil_t * tmp = (fil_t *)realloc(fs->fils, fs->capacite*2);
        if(tmp==NULL){
            perror("realloc");
            return NULL;
        }
        fs->fils = tmp;
        fs->capacite *= 2;
    }
    fil_t f;
    f.num_fil = fs->nb_fils+1;
    memmove(f.origine, orig, LEN_PSEUDO * sizeof(char));
    f.origine[LEN_PSEUDO]='\0';
    f.nb_billets=0;
    f.capacite = CAP_FIL_INIT;
    f.billets = (billet_t*) malloc(CAP_FIL_INIT * sizeof(billet_t));
    if(f.billets == NULL){
        perror("malloc");
        return NULL;
    }
    fs->fils[fs->nb_fils] = f;
    fs->nb_fils+=1;

    return fs->fils + (fs->nb_fils-1);
}

int ajouter_billet(fil_t * fil, char * pseu, uint8_t len, char* text_billet){
    //vérifier si la capacité du tableau de billet est insuffisante
    if(fil->capacite == fil->nb_billets){
        billet_t * tmp = (billet_t *)realloc(fil->billets, fil->capacite*2);
        if(tmp==NULL){
            perror("realloc");
            return FALSE;
        }
        fil->billets = tmp;
        fil->capacite*=2;
    }
    billet_t b;
    b.data_len = len;
    memmove(b.pseudo, pseu, LEN_PSEUDO);
    b.pseudo[LEN_PSEUDO] = '\0';
    b.data = (char *)malloc(sizeof(char) * len + 1);
    if(b.data == NULL){
        perror("malloc");
        return FALSE;
    }
    memmove(b.data, text_billet, len);
    b.data[len] = '\0';

    fil->billets[fil->nb_billets] = b;
    fil->nb_billets +=1; 

    return TRUE;
}

int ajouter_billet_num(fils_t * fs,uint16_t num_f, char * pseu, uint8_t len, char* text_billet){
    if(num_f>fs->nb_fils)//le fil n'existe pas
        return FALSE;
    return ajouter_billet(fs->fils+num_f-1, pseu,len,text_billet);
}

/** retourne 0 si le fil n'existe pas*/
int nb_derniers_billet_du_fil(fils_t * fils, uint16_t numfil,uint16_t nb, uint16_t * nb_rep){
    if(numfil>fils->nb_fils)//le fil numfil n'existe pas
        return 0;
    fil_t fil = fils->fils[numfil-1]; 
    if(nb==0 || nb>=fil.nb_billets){//tous les billets du fil seront envoyés 
        *nb_rep = fil.nb_billets;
        return 1;
    }
    //sinon nb derniers messages du fil seront envoyés
    *nb_rep = nb;
    return 1;
}

/** retourne 0 si le fil demandé n'existe pas*/
int nb_message_a_envoyer(fils_t * fils, uint16_t numfil, uint16_t nb, uint16_t* numfil_rep, uint16_t* nb_rep){
    if(numfil>0){
        *numfil_rep = numfil;
        return nb_derniers_billet_du_fil(fils,numfil,nb,nb_rep);
    }
    //sinon on clacule le nombre de billets à envoyer depuis chaque fil
    *numfil_rep = fils->nb_fils;
    u_int16_t nb_i ;
    *nb_rep=0;
    for(int i=0; i<fils->nb_fils; i++){
        nb_derniers_billet_du_fil(fils,i+1,nb,&nb_i);//le fil existe forcément
        *nb_rep+=nb_i;
    }    
    return 1;
}

/** on suppose que le fil existe, met dans nb_cop le nombre de messages copiés dans messages*/
int get_messages_fil(fils_t * fils, char**messages, int ind_comm, uint16_t numfil, u_int16_t nb, uint16_t* nb_cop){
    fil_t fil = fils->fils[numfil-1];
    int i=0;
    if(nb==0 || nb>=fil.nb_billets){//tous les billets du fil seront envoyés 
        if(nb_cop)
            *nb_cop = fil.nb_billets;
        i=0;
    }
    else{//sinon on cherche les nb derniers billets
        if(nb_cop)
            *nb_cop = nb;
        i = fil.nb_billets - nb;
    }
    for(; i<fil.nb_billets; i++){
        messages[ind_comm+i] = message_billet(numfil,fil.origine,fil.billets[i].pseudo,fil.billets[i].data_len, fil.billets[i].data);
        if(!messages[ind_comm+i])
            return 0;
    }
    return 1;
}

void free_messages_billets(char** messages, uint16_t nb_mess){
    for(int i=0; i<nb_mess; i++){
        if(messages[i])
            free(messages[i]);
    }
    free(messages);
}

int get_messages(fils_t * fils, uint16_t numfil, uint16_t nb, char*** messages, uint16_t* numfil_rep, uint16_t* nb_rep){
    if(!nb_message_a_envoyer(fils, numfil, nb, numfil_rep, nb_rep))// si le fil n'existe pas
        return 0;
    if(*nb_rep==0)
        return 0;
    *messages = (char**)malloc(*nb_rep * sizeof(char*));
    if(!*messages){
        perror("malloc");
        return 0;
    }
    memset(*messages,0,*nb_rep*sizeof(char*));
    if(numfil>0){
        if(!get_messages_fil(fils,*messages,0,numfil,nb,NULL)){
            free_messages_billets(*messages, *nb_rep);
            return 0;
        }
        return 1;
    }
    //on veut les messages de tous les fils
    int ind_comm = 0;
    for(int i=0; i<fils->nb_fils; i++){
        uint16_t nb_cop=0;
        if(!get_messages_fil(fils,*messages,0,numfil,nb,&nb_cop)){
            free_messages_billets(*messages, *nb_rep);
            return 0;
        }
        ind_comm+= nb_cop;
    }
    return 1;
}