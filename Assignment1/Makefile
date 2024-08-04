CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS =

SRC = even.c
OBJ = $(SRC:.c=.o)
EXEC = out

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean run
