CC = gcc
CFLAGS = -g3 -O0 -pthread

all: WRITE BC TERM SCREEN CLIENT SERVER

CLIENT : ./obj/client.o ./obj/bc.o ./obj/term.o ./obj/write.o ./obj/screen.o
	$(CC) ./obj/client.o ./obj/bc.o ./obj/term.o ./obj/write.o ./obj/screen.o -o ./bin/client $(CFLAGS)

./obj/client.o: ./src/client.c
	$(CC) -c ./src/client.c -o ./obj/client.o

SERVER: ./obj/server.o ./obj/bc.o ./obj/term.o ./obj/write.o
	$(CC) ./obj/server.o ./obj/bc.o ./obj/term.o ./obj/write.o -o ./bin/server $(CFLAGS)

./obj/server.o: ./src/server.c
	$(CC) -c ./src/server.c -o ./obj/server.o

BC: ./obj/bc.o

./obj/bc.o: ./src/bc.c
	$(CC) -c ./src/bc.c -o ./obj/bc.o

TERM: ./obj/term.o

./obj/term.o: ./src/term.c
	$(CC) -c ./src/term.c -o ./obj/term.o

WRITE: ./obj/write.o

./obj/write.o: ./src/write.c
	$(CC) -c ./src/write.c -o ./obj/write.o

SCREEN: ./obj/screen.o

./obj/screen.o: ./src/screen.c
		$(CC) -c ./src/screen.c -o ./obj/screen.o

clean:
	rm ./obj/*.o
