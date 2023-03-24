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
    return 0;
}

int choix_client(int sock){
  char offre_inscription[] = "Tapez 1 pour s'inscrire auprès du serveur\n";
  char offre_poster_billet[] = "Tapez 2 pour poster un billet sur un fil\n"; 
  char offre_demande_billets[] = "Tapez 3 pour demander la liste des derniers billets sur un fil\n";
  printf("%s%s%s", offre_inscription, offre_poster_billet, offre_demande_billets);
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
    default:
      fprintf(stderr,"Opération pas prise en charge\n");
      break;
  }
  return 0;
}