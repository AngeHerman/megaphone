#ifndef BUF_H
#define BUF_H
#define SIZE_BUF 4096

typedef struct t {
    char * buf;
    int cur; //La position où on commencera à lire les octets
    int size; //Le nombre total d'octets lu
} buf_t;


/**
 * alloue une struct buf_t de capacité SIZE_BUF + 1
 * @return la structure créee ou NULL en cas d'erreur
 */
buf_t * creer_buf_t ();

/**
 * Vérifie si on a déja assez d'octets dans le buf et copie ses octes dans msg_buf et sinon
 * ca relance la demande de la suite au serveur
 * @param sock la socket serveur
 * @param buf le buffer
 * @param nb_octets_voulu le nombre d'octets qu'on veux lire
 * @return 0 si tout s'est bien passé et -1 sinon
 */
int read_buf(int sock,buf_t* buf,char *msg_buf, int nb_octets_voulu);


/**
 * Fais des recv jusqu'a ce qu'on est le nombre d'octets voulu
 * @param sock la socket serveur
 * @param buf le buffer
 * @param nb_octets_voulu le nombre d'octets qu'on veux lire
 * @return 0 si tout s'est bien passé et -1 sinon
 */
int read_msg(int sock,buf_t* buf,char *msg_buf, int nb_octets_voulu);



#endif
