CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS =

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXEC = program

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean run