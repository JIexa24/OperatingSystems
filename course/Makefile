CC = gcc
CFLAGS = -g3 -O0 -std=c99 

all: CLIENT SERVER

CLIENT : ./obj/client.o
	$(CC) ./obj/client.o -o ./bin/client $(CFLAGS)

./obj/client.o: ./src/client.c
	$(CC) -c ./src/client.c -o ./obj/client.o -std=c99

SERVER: ./obj/server.o
	$(CC) ./obj/server.o -o ./bin/server $(CFLAGS)

./obj/server.o: ./src/server.c
	$(CC) -c ./src/server.c -o ./obj/server.o -std=c99

clean:
	rm ./obj/*.o
