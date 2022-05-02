CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -pedantic -Werror
LFLAGS=-lpthread -lrt -lm
BIN=proj2
SOURCE=main.c

all:
	$(CC) $(CFLAGS) $(SOURCE) -o $(BIN) $(LFLAGS)

	./$(BIN) 6 4 100 100




