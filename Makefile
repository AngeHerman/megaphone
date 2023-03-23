main_client : main_client.o messages_client.o client.o buf.o interaction.o
	gcc main_client.o client.o messages_client.o buf.o interaction.o -o main_client 
main_client.o : main_client.c client.h 
	gcc -c main_client.c -o main_client.o
client.o : client.c client.h messages_client.h
	gcc -c client.c -o client.o
interaction.o : interaction.c client.h interaction.h
	gcc -c interaction.c -o interaction.o
messages_client.o : messages_client.c messages_client.h
	gcc -c messages_client.c -o messages_client.o
buf.o : buf.c buf.h
	gcc -c buf.c -o buf.o

main_serveur : main_serveur.o messages_serveur.o serveur.o inscrits.o fils.o lecture.o
	gcc main_serveur.o messages_serveur.o serveur.o inscrits.o fils.o lecture.o -pthread -o main_serveur
main_serveur.o : main_serveur.c serveur.h lecture.h 
	gcc -c main_serveur.c -o main_serveur.o
serveur.o : serveur.c serveur.h lecture.h
	gcc -c serveur.c -o serveur.o
messages_serveur.o : messages_serveur.c messages_serveur.h
	gcc -c messages_serveur.c -o messages_serveur.o
fils.o : fils.c fils.h
	gcc -c fils.c -o fils.o
inscrits.o : inscrits.c inscrits.h
	gcc -c inscrits.c -o inscrits.o

lecture.o : lecture.c lecture.h 
	gcc -c lecture.c -o lecture.o
clean : 
	rm -rf  main_client main_serveur *.o
