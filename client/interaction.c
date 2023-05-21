#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <readline/readline.h>
#include <limits.h>

#include "client.h"
#include "interaction.h"
#include "abonnement.h"



int inscrire_client(int sock){
    char pseudo[11];
    memset(pseudo,'#',10);
    pseudo[10]='\0';
    char * pseudo_rep = readline("Entrez votre pseudo (entre 1 et 10 caractères) :\n");
    if(!pseudo_rep){
        perror("readline");
        return 0;
    }
    memmove(pseudo, pseudo_rep, strlen(pseudo_rep)<=10 ? strlen(pseudo_rep) : 10);//on prends les 10 premiers caractères
    printf("pseudo : %s\n", pseudo);
    free(pseudo_rep);
    uint16_t id = demande_inscription(sock, pseudo);
    if(id){
        printf("inscription réussie. ID attribué : %d\n", id);
        return 1;
    }
    fprintf(stderr, "Echec, le serveur n'a pas pu vous inscrire\n");
    return 0;
}

int poster_billet(int sock){
    uint16_t id;
    uint16_t numfil;
    uint8_t datalen;
    char texte[255+1];
    memset(texte,0,sizeof(texte));
    char * line = NULL;
    line = readline("Entrez votre id :\n");
    if(!line){
        perror("readline");
        return 0;
    }
    id = atoi(line);
    free(line);
    line = readline("Entrez le numéro de fil ou 0 pour poster le billet sur un nouveau fil :\n");
    if(!line){
        perror("readline");
        return 0;
    }
    numfil = atoi(line);
    free(line);
    line = readline("Entrez le texte du billet :\n");
    if(!line){
        perror("readline");
        return 0;
    }
    memmove(texte,line,strlen(line)<=255 ? strlen(line) : 255);//enlever le '\n'
    datalen = strlen(texte);
    free(line);
    int ret = poster_un_billet(sock, id, numfil, datalen, texte);
    if(!ret)
        fprintf(stderr,"Le billet n'est pas posté\n");
    else
        printf("Le billet est posté sur le fil numéro %u\n", ret);
    return ret;
}

int demader_billets(int sock){
    int id;
    int numfil;
    int nb_billets;
    char * line = NULL;
    line = readline("Entrez votre id :\n");
    if(! line){
        perror("readline");
        return 0;
    }
    id = atoi(line);
    free(line);
    line = readline("Entrez le numéro de fil ou 0 pour demander les billets de tous les fils :\n");
    if(! line){
        perror("readline");
        return 0;
    }
    numfil = atoi(line);  
    free(line);
    line = readline("Entrez le nombre de billet voulu :\n");
    if(! line){
        perror("readline");
        return 0;
    }
    nb_billets = atoi(line);
    free(line); 
    int ret = demande_dernier_billets(sock, id, numfil, nb_billets);
    if(ret==0){
        fprintf(stderr,"La demande a échouée\n");
        return 0;
    }
    return 1;
}

int abonnement_fil(int sock){
	uint16_t id;
	uint16_t numfil;
    
    char * line = NULL;
    line = readline("Entrez votre id :\n");
    if(! line){
        perror("readline");
        return 0;
    }
    id = atoi(line);
    free(line);
	line = readline("Entrez le numéro de fil ou 0 pour vous abonner à tous les fils:\n");
    if(! line){
        perror("readline");
        return 0;
    }
    numfil = atoi(line);  
    free(line);
    int ret = demande_abonnement(sock, id, numfil);
    if(!ret)
        fprintf(stderr,"demande d'abonnement echouée\n");
    else
        printf("demande d'abonnement effectuée\n");
    return ret;
}

char * get_file_name(char * path){
    if(path==NULL || strlen(path)<1)
        return NULL;
    int i;
    for(i = strlen(path)-1; i>0; i--){
        if(path[i]=='/')
            break;
    }
    if(path[i]=='/')
        i++;
    if(i>=strlen(path)-1)
        return NULL;
    char * res = (char *) malloc(sizeof(char) * (strlen(path)-i+1));
    memset(res, 0, sizeof(char) * (strlen(path)-i+1));
    if(!res){
        perror("malloc");
        return NULL;
    }
    memmove(res, path+i,strlen(path)-i);
    return res;
}

int ajouter_fichier(int sock,char* hostname){
	uint16_t id;
	uint16_t numfil;
	uint8_t datalen;
	char file_path[PATH_MAX];
    memset(file_path,0,sizeof(file_path));
    char * line; 
    
    line = readline("Entrez votre id :\n");
    if(!line){
        perror("readline");
        return 0;
    }
    id = atoi(line);
    free(line);
    
    line = readline("Entrez le numéro du fil où envoyer le fichier :\n");
    if(!line){
        perror("readline");
        return 0;
    }
    numfil = atoi(line);
    free(line);
    
    line = readline("Entrez le chemin du fichier à envoyer :\n");
    if(!line){
        perror("readline");
        return 0;
    }
    memmove(file_path,line,strlen(line)<=PATH_MAX ? strlen(line) : PATH_MAX);
    free(line);
    char * file_name = get_file_name(file_path);
    if(!file_name){
        fprintf(stderr,"erreur path\n");
        return 0;
    }
    datalen = strlen(file_name);
	int ret = ajouter_un_fichier(sock, id, numfil, datalen, file_name, hostname, file_path);
    free(file_name);
    if(!ret)
        fprintf(stderr,"Le fichier n'a pas été ajouté\n");
    else
        printf("la demande de transfert de fichier sur le fil numéro %u a été effectuée\n", numfil);
    return 1;
}

int telecharger_fichier(int sock){
    uint16_t id;
	uint16_t numfil;
	uint8_t datalen;
	char file_name[PATH_MAX];
    memset(file_name,0,sizeof(file_name));
    char * line; 
    
    line = readline("Entrez votre id :\n");
    if(!line){
        perror("readline");
        return 0;
    }
    id = atoi(line);
    free(line);
    
    line = readline("Entrez le numéro du fil où telecharger le fichier :\n");
    if(!line){
        perror("readline");
        return 0;
    }
    numfil = atoi(line);
    free(line);
    
    line = readline("Entrez le nom du fichier à telecharger :\n");
    if(!line){
        perror("readline");
        return 0;
    }
    memmove(file_name,line,strlen(line)<=PATH_MAX ? strlen(line) : PATH_MAX);
    free(line);
    datalen = strlen(file_name);
	int ret = telecharger_un_fichier(sock, id, numfil, datalen, file_name);
    if(!ret)
        fprintf(stderr,"Le fichier n'a pas été telechargé\n");
    else
        printf("la demande de téléchargement de fichier du fil numéro %u a été effectuée\n", numfil);
    return 1;
}

int choix_client(int sock,char* hostname){
	char choix[] = "Tapez 1 pour s'inscrire auprès du serveur\nTapez 2 pour poster un billet sur un fil\nTapez 3 pour demander la liste des derniers billets sur un fil\nTapez 4 pour vous abonner à un fil\nTapez 5 pour ajouter un fichier sur un fil\nTapez 6 pour telecharger un fichier\n";
    pthread_mutex_lock(&verrou_affichage);
	char* rep_char = readline(choix);
    pthread_mutex_unlock(&verrou_affichage);
    if(rep_char==NULL){
        perror("readline");
        return 0;
    }
	int rep_client = atoi(rep_char);
    free(rep_char);
	switch (rep_client){
		case 1:
			return inscrire_client(sock);
		case 2 :
			return poster_billet(sock);
		case 3 : 
			return demader_billets(sock);
		case 4:
			return abonnement_fil(sock);
		case 5 :
			return ajouter_fichier(sock,hostname);
		case 6 : 
			return telecharger_fichier(sock);
		default:
			fprintf(stderr,"Opération pas prise en charge\n");
			break;
	}
	return 0;
}