CC = gcc
CLFAGS = -Wall -Wextra -O2 -std=c99

SOURCE = linecount.c
OBJ = linecount

.PHONY: all clean

all:
	$(CC) $(CLFAGS) -o $(OBJ) $(SOURCE)

clean:
	rm -rf $(OBJ)
