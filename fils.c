#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#include "fils.h"

#define CAP_FILS_INIT 10
#define CAP_FIL_INIT 10
#define LEN_PSEUDO 10
#define FALSE 0
#define TRUE 1


fils_t * creer_list_fils(){
    fils_t * res = malloc(sizeof(fils_t));
    if(res == NULL){
        perror("malloc");
        return NULL;
    }
    memset(res,0,sizeof(fils_t));
    res->capacite = CAP_FILS_INIT;
    res->fils = malloc(CAP_FILS_INIT * sizeof(fil_t));
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
        fil_t * tmp = realloc(fs, fs->capacite*2);
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
    f.billets = malloc(CAP_FIL_INIT * sizeof(billet_t));
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
        billet_t * tmp = realloc(fil->billets, fil->capacite*2);
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
    b.data = malloc(sizeof(char) * len + 1);
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