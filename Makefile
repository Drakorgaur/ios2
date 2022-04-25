CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -pedantic # removed -Werror
LFLAGS=-lpthread -lrt -lm
BIN=project4
SOURCE=main.c

main:
	$(CC) $(CFLAGS) $(SOURCE) -o $(BIN) $(LFLAGS)

	./$(BIN) 3 8 1000 100



