
CC=gcc
CFLAGS=-O3

converter: main.o
	$(CC) -o converter main.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) 

clean:
	rm -f *.o converter 

all: converter test

