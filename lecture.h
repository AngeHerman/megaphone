#ifndef LECTURE_H
#define LECTURE_H

typedef struct t {
    char * buf;
    int current; // position courante dans le buffer
    int expected_size; //Le nombre d'octets à lire
} buf_t;


/**
 * alloue une struct buf_t de capacité size
 * @return la structure créee ou NULL en cas d'erreur
 */
buf_t * creer_buf_t (int size);


/**
 * Fais des recv jusqu'a ce qu'on est le nombre d'octets attendu
 * @param sock la socket serveur
 * @param buf le buffer
 * @param nb_octets_voulu le nombre d'octets qu'on veux lire
 * @return 1 si tout s'est bien passé et 0 sinon
 */
int read_msg(int sock,buf_t* buf);



#endif
