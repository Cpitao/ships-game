#include <stdio.h>
#include <string.h>
#include "sql_queries.h"

#define BOARD_SIZE 10

#define HIT_MOVE 1
#define MISS_MOVE -1
#define INVALID_MOVE 0

#define SERVER_MOVE 0
#define CLIENT_MOVE 1

enum CellStatus {
    EMPTY,
    SHIP,
    HIT,
    MISS,
    PENDING
};

typedef struct {
    enum CellStatus grid[BOARD_SIZE + 2][BOARD_SIZE + 2];
} Board;

typedef struct {
    Board player_board;
    Board server_board;
    int move;
} Game;

int shoot(Board *board, int x, int y);
int check_win(Board *board);
int mask_board(Board *board, Board* masked_board);
void board_to_string(Board* board, char* dest);
void game_to_string(Game* board, char* dest);
