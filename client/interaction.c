#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "client.h"
#include "interaction.h"



int inscrire_client(int sock){
    char pseudo[11];
    memset(pseudo, '#', 10);
    printf("Entrez votre pseudo (entre 1 et 10 caractères) :\n");
    scanf("%s", pseudo);
    pseudo[10] = '\0';

    uint16_t id = demande_inscription(sock, pseudo);
    if(id){
        printf("inscription réussie. ID attribué : %d\n", id);
        return 1;
    }
    printf("Echec, le serveur n'a pas pu vous inscrire\n");
    return 0;
}

int poster_billet(int sock){
    int id;
    int numfil;
    uint8_t datalen;
    char texte[255+1];
    memset(texte,0,sizeof(texte));
    char temp[255+1+1];
    memset(temp,0,sizeof(temp));
    char nb[7]={0};
    printf("Entrez votre id :\n");
    fgets(nb,7,stdin);
    id = atoi(nb);
    memset(nb,0,sizeof(nb));
    printf("Entrez le numéro de fil ou 0 pour poster le billet sur un nouveau fil :\n");
    fgets(nb,7,stdin);
    numfil = atoi(nb);
    printf("Entrez le texte du billet :\n");
    fgets(temp, 257, stdin);
    memmove(texte,temp,strlen(temp)-1);//enlever le '\n'
    datalen = strlen(texte);
    
    uint16_t res = poster_un_billet(sock, id, numfil, datalen, texte);
    if(res==0){
        fprintf(stderr,"Le billet n'est pas posté\n");
        return 0;
    }
    printf("Le billet est posté sur le fil numéro %u\n", res);
    return 1;
}

int demader_billets(int sock){
    int id;
    int numfil;
    int nb;
    printf("Entrez votre id :\n");
    scanf("%d", &id);
    printf("Entrez le numéro de fil ou 0 pour demander les billets de tous les fils:\n");
    scanf("%d", &numfil);
    printf("Entrez le nombre de billet voulu :\n");
    scanf("%d", &nb);
    
    int res = demande_dernier_billets(sock, id, numfil, nb);
    if(res==0){
        fprintf(stderr,"La demande a échouée\n");
        return 0;
    }
    // printf("Demande réussi %u\n", res);
    return 1;
}

int abonnement_fil(int sock){
	int id;
	int numfil;

	printf("Entrez votre id :\n");
	scanf("%d", &id);
	printf("Entrez le numéro de fil ou 0 pour demander les billets de tous les fils:\n");
	scanf("%d", &numfil);


}

int ajouter_fichier(int sock,char* hostname){
	int id;
	int numfil;
	uint8_t datalen;
	char file_name[255+1];
    char tmp[255+1+1];
    char nb[7];
    memset(file_name,0,sizeof(file_name));
    memset(tmp,0,sizeof(tmp));
    
    printf("Entrez votre id :\n");
    fgets(nb,7,stdin);
    id = atoi(nb);
    memset(nb,0,sizeof(nb));
    printf("Entrez le numéro du fil où envoyer le fichier :\n");
    fgets(nb,7,stdin);
    numfil = atoi(nb);
    printf("Entrez le nom du fichier à envoyer :\n");
    fgets(tmp, 257, stdin);
    memmove(file_name,tmp,strlen(tmp)-1);//enlever le '\n'
    datalen = strlen(file_name);
    
	uint16_t res = ajouter_un_fichier(sock, id, numfil, datalen, file_name, hostname);
    if(res==0){
        fprintf(stderr,"Le fichier n'a pas été ajouté\n");
        return 0;
    }
    printf("Le fichier a été posté sur le fil numéro %u\n", res);
    return 1;

}

int telecharger_fichier(int sock){
    return 0;
}

int choix_client(int sock,char* hostname){
	char inscription[] = "Tapez 1 pour s'inscrire auprès du serveur\n";
	char poster_un_billet[] = "Tapez 2 pour poster un billet sur un fil\n"; 
	char demande_billets[] = "Tapez 3 pour demander la liste des derniers billets sur un fil\n";
	char abonnement[] = "Tapez 4 pour vous abonner à un fil\n";
	char ajout_fichier[] = "Tapez 5 pour ajouter un fichier sur un fil\n";
	char tlc_fichier[] = "Tapez 6 pour telecharger un fichier\n";

	printf("%s%s%s%s%s%s", inscription, poster_un_billet, demande_billets,abonnement,ajout_fichier,tlc_fichier);
	int rep_client;
	char rep_s[3] = {0};
	fgets(rep_s, 3, stdin);
	rep_client = atoi(rep_s);
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