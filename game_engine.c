#include "game_engine.h"

int shoot(Board* board, int x, int y) {
    switch(board->grid[x][y]) {
        case EMPTY:
            board->grid[x][y] = MISS;
            return MISS_MOVE;
        case SHIP:
            board->grid[x][y] = HIT;
            return HIT_MOVE;
        default:
            return INVALID_MOVE;
    }
}

int check_win(Board* board) {
    for (int i=0; i < BOARD_SIZE; i++) {
        for (int j=0; j < BOARD_SIZE; j++) {
            if (board->grid[i][j] == SHIP) return 0;
        }
    }
    return 1;
}

int mask_board(Board* board, Board* masked_board) {
    for (int i=0; i < BOARD_SIZE; i++) {
        for (int j=0; j < BOARD_SIZE; j++) {
            masked_board->grid[i][j] = board->grid[i][j];
            if (masked_board->grid[i][j] == SHIP)
                masked_board->grid[i][j] = EMPTY;
        }
    }
    return 0;
}

void board_to_string(Board* board, char* dest) {
    for (int i=0; i < BOARD_SIZE; i++) {
        for (int j=0; j < BOARD_SIZE; j++) {
            char cell_char;
            switch (board->grid[i][j]) {
                case EMPTY:
                    cell_char = ' ';
                    break;
                case SHIP:
                    cell_char = 'S';
                    break;
                case HIT:
                    cell_char = 'X';
                    break;
                case MISS:
                    cell_char = 'O';
                    break;
                case PENDING:
                    cell_char = 'P';
                    break;
            }
            dest[i * BOARD_SIZE + j] = cell_char;
        }
    }

    dest[BOARD_SIZE * BOARD_SIZE] = '\0';
}

int valid_position_for_ship(Board* board, int* position) {
    int x = position[0];
    int y = position[1];

    if (board->grid[x][y] == SHIP || board->grid[x][y] == PENDING) return 0;
    int shifts[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}; 
    for (int s=0; s < 8; s++) {
        if (board->grid[x - shifts[s][0]][y - shifts[s][0]] == SHIP) {
            return 0;
        }
    }
    return 1;
}

int place_ship(Board* board, int **positions, int size) {
    for (int i=0; i < size; i++) {
        if (!valid_position_for_ship(board, positions[i])) return -1;
    }
    for (int i=0; i < size; i++) {
        board->grid[positions[i][0]][positions[i][1]] = SHIP;
    }

    return 0;
}

void play_game(Game* game, User* player) {
    
}