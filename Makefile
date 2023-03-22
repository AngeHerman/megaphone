main_client : main_client.o messages_client.o client.o
	gcc main_client.o client.o messages_client.o -o main_client 
main_serveur : main_serveur.o messages_serveur.o serveur.o inscrits.o 
	gcc main_serveur.o messages_serveur.o serveur.o inscrits.o -o main_serveur
main_client.o : main_client.c client.h
	gcc -c main_client.c -o main_client.o
client.o : client.c client.h messages_client.h
	gcc -c client.c -o client.o
messages_client.o : messages_client.c messages_client.h
	gcc -c messages_client.c -o messages_client.o
messages_serveur.o : messages_serveur.c messages_serveur.h
	gcc -c messages_serveur.c -o messages_serveur.o
test : messages_client.o test.o inscrits.o fils.o
	gcc test.o messages_client.o inscrits.o fils.o -o test
test.o : test.c messages_client.h inscrits.h
	gcc -c test.c -o test.o
fils.o : fils.c fils.h
	gcc -c fils.c -o fils.o
inscrits.o : inscrits.c inscrits.h
	gcc -c inscrits.c -o inscrits.o
clean : 
	rm -rf test main_client main_serveur *.o
main_serveur.o : main_serveur.c serveur.h 
	gcc -c main_serveur.c -o main_serveur.o
serveur.o : serveur.c serveur.h
	gcc -c serveur.c -o serveur.o
