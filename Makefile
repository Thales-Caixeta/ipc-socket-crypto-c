# Makefile
# Aluno: Thales Moura Machado Caixeta
CC=gcc
CFLAGS=-Wall -Wextra -O2

all: server client

server: server.c crypto.c
	$(CC) $(CFLAGS) server.c crypto.c -o server

client: client.c crypto.c
	$(CC) $(CFLAGS) client.c crypto.c -o client

clean:
	rm -f server client
   