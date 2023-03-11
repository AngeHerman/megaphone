main_client : main_client.o messages.o client.o
	gcc main_client.o client.o messages.o -o main_client
main_client.o : main_client.c client.h
	gcc -c main_client.c -o main_client.o
client.o : client.c client.h messages.h
	gcc -c client.c -o client.o
messages.o : messages.c messages.h
	gcc -c messages.c -o messages.o

test : messages.o test.o inscrits.o fils.o
	gcc test.o messages.o inscrits.o fils.o -o test
test.o : test.c messages.h inscrits.h
	gcc -c test.c -o test.o
fils.o : fils.c fils.h
	gcc -c fils.c -o fils.o
inscrits.o : inscrits.c inscrits.h
	gcc -c inscrits.c -o inscrits.o
clean : 
	rm test main_client *.o
