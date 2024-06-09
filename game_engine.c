#include "game_engine.h"

int shoot(Board* board, int x, int y) {
    switch(board->grid[x][y]) {
        case CellStatus.EMPTY:
            board->grid[x][y] = CellStatus.MISS;
            return MISS;
        case CellStatus.SHIP:
            board->grid[x][y] = CellStatus.HIT;
            return HIT;
        default:
            return INVALID_MOVE;
    }
}

int check_win(Board* board) {
    for (int i=0; i < BOARD_SIZE; i++) {
        for (int j=0; j < BOARD_SIZE; j++) {
            if (board->grid[i][j] == CellStatus.SHIP) return 0;
        }
    }
    return 1;
}

int mask_board(Board* board, Board* masked_board) {
    for (int i=0; i < BOARD_SIZE; i++) {
        for (int j=0; j < BOARD_SIZE; j++) {
            masked_board->grid[i][j] = board->grid[i][j];
            if (masked_board->grid[i][j] == CellStatus.SHIP)
                masked_board->grid[i][j] = CellStatus.EMPTY;
        }
    }
    return masked_board;
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
            }
            dest[i * BOARD_SIZE + j] = cell_char;
        }
    }

    dest[BOARD_SIZE * BOARD_SIZE] = '\0';
}