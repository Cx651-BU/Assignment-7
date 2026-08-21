all: matrix

CC=gcc
CFLAGS=-I. -lm

%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

client: client.o
	$(CC) -o $@ $^ $(CFLAGS)

server: server.o game.o save.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o
	rm matrix