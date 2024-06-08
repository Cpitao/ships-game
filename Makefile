CC = gcc
CFLAGS = -Wall -g
TARGET = server
SRC = server.c sql_queries.c client_handler.c
OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lsqlite3

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean
