#ifndef ABONNEMENT_H
#define ABONNEMENT_H
#include <pthread.h>

extern pthread_mutex_t verrou_affichage;

typedef struct info_abonn{
    struct in6_addr addr_diffus;
    uint16_t port;
}info_abonn;

void * abonnement(void * args);

#endif  
