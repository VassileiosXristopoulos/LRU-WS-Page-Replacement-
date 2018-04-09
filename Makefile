CC = gcc # Compiler

all:src/program

src/program: src/main.o src/ws.o src/functions.o src/lru.o
	$(CC) -o program main.o ws.o functions.o lru.o -lm

src/main.o:
	$(CC) -c src/main.c

src/ws.o:
	$(CC) -c src/ws.c

src/functions.o:
	$(CC) -c src/functions.c

src/lru.o:
	$(CC) -c src/lru.c

.Phony: clean

clean:
	rm -r program main.o ws.o functions.o lru.o
