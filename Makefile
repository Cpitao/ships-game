CC = gcc
CFLAGS = -Wall -g

SERVER_TARGET = server
CLIENT_TARGET = client

SERVER_SRC = server.c sql_queries.c client_handler.c game_server.c game_engine.c utils.c messages.c
CLIENT_SRC = client.c game_client.c utils.c messages.c

SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

# Default target
all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lsqlite3

$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ) $(SERVER_TARGET) $(CLIENT_TARGET)

.PHONY: clean
