CC = gcc
CFLAGS = -Wall -c

run_client: client main_client run 

client :
	$(CC) $(CFLAGS) client.c
	
main_client:
	$(CC) $(CFLAGS) main_client.c

run:
	$(CC) client.o main_client.o -o main_client

clean: 
	$(RM) -rf *.o main_client