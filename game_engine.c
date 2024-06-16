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
    int dest_index = 0;
    dest[dest_index++] = ' ';
    dest[dest_index++] = ' ';
    for (int i=0; i < BOARD_SIZE - 1; i++) {
        dest[dest_index++] = '0' + i + 1;
    }
    dest[dest_index++] = '1';
    dest[dest_index++] = '0';
    dest[dest_index++] = '\n';
    for (int i=0; i < BOARD_SIZE + 2; i++) {
        dest[dest_index++] = '_';
    }
    dest[dest_index++] = '\n';
    for (int i = 1; i < BOARD_SIZE + 1; i++) {
        dest[dest_index++] = 'A' + i - 1;
        dest[dest_index++] = '|';
        for (int j = 1; j < BOARD_SIZE + 1; j++) {
            char cell_char;
            switch (board->grid[i][j]) {
                case EMPTY:
                    cell_char = '-';
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
            dest[dest_index++] = cell_char;
        }
        dest[dest_index++] = '\n';
    }
    dest[dest_index] = '\0';
}
