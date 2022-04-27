CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
LFLAGS=-lpthread -lrt -lm
BIN=proj2
SOURCE=main.c

main:
	$(CC) $(CFLAGS) $(SOURCE) -o $(BIN) $(LFLAGS)

	./$(BIN) 2 2 100 100




