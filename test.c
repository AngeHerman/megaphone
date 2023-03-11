#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "messages.h"
#include "inscrits.h"
#include "fils.h"

int main(){

    printf("%d\n", ntohs(entete_message(20,20)));
    exit(0);
}