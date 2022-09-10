CC = gcc
CLFAGS = -Wall -Wextra -O2 -std=c99

SOURCE = linecount.c
OBJ = linecount

.PHONY: all clean

all:
	$(CC) $(CLFAGS) -o $(OBJ) $(SOURCE)

debug:
	$(CC) $(CLFAGS) -g -o $(OBJ)_dbg $(SOURCE)

clean:
	rm -rf $(OBJ)
	rm -rf $(OBJ)_dbg
