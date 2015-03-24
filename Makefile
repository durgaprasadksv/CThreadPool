CC=gcc
CFLAGS= -g -O2 -Wall
LIB=-lpthread

all: test example

example: thpool.o example.c
	$(CC) $(CFLAGS) thpool.o example.c -o example $(LIB)

test: thpool.o test.c
	$(CC) $(CFLAGS) thpool.o test.c -o test $(LIB)

thpool.o:
	$(CC) $(CFLAGS) -c thpool.c

clean:
	rm -fr *.o
