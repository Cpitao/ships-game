#include "utils.h"
#include "messages.h"
#include "game_server.h"
#include <stdio.h>

void handle_client(sqlite3 *db, int connfd);