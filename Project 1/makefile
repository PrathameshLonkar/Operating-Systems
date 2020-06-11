CC=gcc
CFLAGS=-Wall -pedantic

all: sh550

sh550: sh550.c
	$(CC) -c sh550.c -o sh550.o
	$(CC) sh550.o -o sh550
	
sh550.c: 
	./sh550
	

clean:
	rm sh550 *.o
