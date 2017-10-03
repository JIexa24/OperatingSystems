CC = gcc

.PHONY: all COMPILE

all: COMPILE 

COMPILE: ./obj/sh.o
	$(CC) ./obj/sh.o -o ./bin/sh 
	$(CC) ./src/cmd-1.c -o ./test-1 
	$(CC) ./src/cmd-2.c -o ./test-2 

./obj/sh.o: ./src/sh.c
	$(CC) -c ./src/sh.c -o ./obj/sh.o

clean:
	rm -f ./obj/*.o

