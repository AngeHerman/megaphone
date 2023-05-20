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
#include <netdb.h>

#include "fichiers.h"



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
    if(len<TAILLE_PAQUET_UDP){
        fic->num_dernier_bloc = bloc->num_bloc;
        printf("num dernier paquet reçu %d\n", bloc->num_bloc);
    }
    
    if(fic->num_dernier_bloc == fic->nb_bloc_recus){//on a eu tout les blocs
        save_fic(fic);
        return FALSE;
    }
    return TRUE;
}

uint8_t fichier_code_req(uint16_t entete){
    uint16_t masque = 0b0000000000011111;
    uint8_t cod_req = entete & masque;
    return cod_req;
}

uint16_t fichier_id_requete(uint16_t entete){
    uint16_t masque = 0b0000000000011111;
    uint16_t id = (entete & ~masque) >> 5;
    return id;
}

int reception_par_paquets_de_512(int sock_udp, char* name_file, uint16_t id, uint16_t numfil){
    char buff[TAILLE_PAQUET_UDP+5]={0};
    int nb=0;//la taille du dernier paquet reçu
    int taille_fic=0;//taille du fichier reçu
    int continuer = TRUE;//on n'a pas encore reçu tous les paquets
    fic_t * fic;
    if(!(fic = init_fic(id, name_file, numfil)))
        return -1;
    while(continuer){
        nb=recvfrom(sock_udp,buff,TAILLE_PAQUET_UDP+5,0,NULL,NULL);
        if(nb==-1){
            perror("recvfrom");
            continue;
        }
        buff[nb]='\0';
        uint16_t entete = ntohs(*(uint16_t*)buff);
        uint16_t num_bloc = ntohs(*(((uint16_t*)buff)+1));
        char * data = buff+4;
        uint8_t code_req = fichier_code_req(entete);
        uint16_t id = fichier_id_requete(entete);
        if(code_req!=5){
            perror("codereq error");
            continue;
        }
        taille_fic+=nb-4;
        continuer = add_bloc(fic,num_bloc,data, nb-4);      
    }
    return taille_fic;
}

long int taille_fichier(char *file_name){
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

int get_server_addrudp(char* hostname, char* port, int * sock_udp, struct sockaddr_in6* addr, int* addrlen) {
    struct addrinfo hints, *r, *p;
    int ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_V4MAPPED | AI_ALL;

    if ((ret = getaddrinfo(hostname, port, &hints, &r)) != 0 || NULL == r){
        fprintf(stderr, "erreur getaddrinfo : %s\n", gai_strerror(ret));
        return -1;
    }
  
    *addrlen = sizeof(struct sockaddr_in6);
    p = r;
    while( p != NULL ){
        if((*sock_udp = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) > 0)
            break;
        close(*sock_udp);
        p = p->ai_next;
    }

    if (NULL == p) return -2;
    *addr = *((struct sockaddr_in6 *) p->ai_addr);
    freeaddrinfo(r);
    return 1;
}

uint16_t entete_mess(uint16_t code_req, uint16_t id){
    uint16_t res = 0;

    res+=code_req;
    id = id<<5;
    res+=id;
    
    return htons(res);
}
char * message_client_udp(uint16_t code_req, uint16_t id,uint16_t numbloc, int datalen, char * data){
	//7 octets + le nombre d'octets pour le texte du billet
	char * res = (char *)malloc((4 + datalen ) * sizeof(char));
	if(res == NULL){
		perror("malloc");
		return NULL;
	}
	//remplir l'entête
	((uint16_t *)res)[0] = entete_mess(code_req,id);
	//les autres champs
	((uint16_t *)res)[1] = htons(numbloc);

	//copier le texte du message
	if(datalen > 0){
		memmove(res+4, data, datalen);    
	}

	return res;
}

int envoi_par_paquets_de_512(int fd, int sock,int id,int taille_fic, struct sockaddr_in6 addr, int adrlen){
    int nb_paquets = taille_fic/TAILLE_PAQUET_UDP;
    printf("%d\n", nb_paquets+1);
    //On lit et on envoie par 512. La boucle fera nb_paquets + 1 tour. Le dernier tour correspondra au dernier paquet <512
    //pour signifier la fin de l'envoie
    for(int i = 0; i <= nb_paquets; i++){
        int taille_a_lire = TAILLE_PAQUET_UDP;
        
        //Le cas où on doit envoyer le dernier paquets < 512 octets
        if(i == nb_paquets){
            taille_a_lire = taille_fic%TAILLE_PAQUET_UDP; //La taille du dernier paquet   soit 0 soit < 512
        }
        char data[taille_a_lire+1];
        memset(data,0,taille_a_lire);
        if(read(fd, data, taille_a_lire) != taille_a_lire){
            perror("read");
            return 0;
        }
        data[taille_a_lire]='\0';
        char * mess_paquet_udp = message_client_udp(CODE_REQ_AJOUT_FICHIER,id,i+1,taille_a_lire,data);
        int nb;
        if((nb=sendto(sock, mess_paquet_udp, 4 + taille_a_lire, 0,(struct sockaddr *) &addr, adrlen)) != 4 + taille_a_lire){
            perror("sendto");
            free(mess_paquet_udp);
            return 0;
        }
        free(mess_paquet_udp);
        sleep(0.1);
    }
    return 1;
}

int envoi_fichier(uint16_t id, uint16_t port,char * file_path,char * hostname){
    printf("file_path : %s\n", file_path);
    struct sockaddr_in6 server_addr;
    int sockUDP, adrlen;
    char port_en_char[10];
    memset(port_en_char,0,10);
    sprintf(port_en_char, "%u", port);
    printf("port:%s\n", port_en_char);
    if(!get_server_addrudp(hostname,port_en_char, &sockUDP, &server_addr, &adrlen))
        return 0;
    int fd= open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("erreur open");
        return 0;
    }
    long int taille_fic = taille_fichier(file_path);
    if(taille_fic == 0)
        return 0;
    if(taille_fic >= TAILLE_MAX_AJOUT_FICHIER){
        fprintf(stderr,"Fichier trop gros, taille max est %d\n",TAILLE_MAX_AJOUT_FICHIER);
        return 0;
    }
    if(!envoi_par_paquets_de_512(fd,sockUDP,id,taille_fic,server_addr,adrlen)){
        return 0;
    }
    return 1;
    
}