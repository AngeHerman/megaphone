test : messages.o test.o inscrits.o fils.o
	gcc test.o messages.o inscrits.o fils.o -o test
fils.o : fils.c fils.h
	gcc -c fils.c -o fils.o
test.o : test.c messages.h inscrits.h
	gcc -c test.c -o test.o
inscrits.o : inscrits.c inscrits.h
	gcc -c inscrits.c -o inscrits.o
messages.o : messages.c messages.h
	gcc -c messages.c -o messages.o
clean : 
	rm test *.o
