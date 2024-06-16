#include "game_engine.h"

int shoot(Board* board, int x, int y) {
    switch(board->grid[x][y]) {
        case EMPTY:
            board->grid[x][y] = MISS;
            return MISS_MOVE;
        case SHIP:
            board->grid[x][y] = HIT;
            break;
        default:
            return INVALID_MOVE;
    }

    int to_check[4][2];
    to_check[0][0] = x;
    to_check[0][1] = y;
    int idx=0;
    int to_check_len = 1;
    int shifts[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int sank = 1;
    while (to_check_len > idx && idx < 4) {
        for (int i=0; i < 4; i++) {
            if (board->grid[to_check[idx][0] + shifts[i][0]][to_check[idx][1] + shifts[i][1]] == SHIP) {
                sank = 0;
                break;
            } else if (board->grid[to_check[idx][0] + shifts[i][0]][to_check[idx][1] + shifts[i][1]] == HIT) {
                int already_in = 0;
                for (int j=0; j < to_check_len; j++) {
                    if (to_check[j][0] == to_check[idx][0] + shifts[i][0] &&
                        to_check[j][1] == to_check[idx][1] + shifts[i][1]) {
                            already_in = 1;
                            break;
                        }
                }
                if (already_in) continue;
                printf("To check: %d %d %d %d\n", x, y, to_check[idx][0] + shifts[i][0], to_check[idx][1] + shifts[i][1]);
                to_check[to_check_len][0] = to_check[idx][0] + shifts[i][0];
                to_check[to_check_len++][1] = to_check[idx][1] + shifts[i][1];
            }
        }
        idx++;
        if (!sank) break;
    }

    if (sank) {
        for (int i=0; i < to_check_len; i++) {
            printf("%d %d\n", to_check[i][0], to_check[i][1]);
            int all_shifts[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1,-1}, {1, 0}, {1, 1}};
            for (int j=0; j < 8; j++) {
                if (board->grid[to_check[i][0] + all_shifts[j][0]][to_check[i][1] + all_shifts[j][1]] != HIT) {
                    board->grid[to_check[i][0] + all_shifts[j][0]][to_check[i][1] + all_shifts[j][1]] = MISS;
                }
            }
        }
    }

    return HIT_MOVE;
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

void game_to_string(Game* game, char* dest) {
    int dest_index = 0;
    int board_size = BOARD_SIZE;
    char move[256];
    if (game->move == SERVER_MOVE) {
        sprintf(move, "Server move\n");
    } else {
        sprintf(move, "Your move\n");
    }
    for (int i=0; i < strlen(move); i++) {
        dest[dest_index++] = move[i];
    }

    dest[dest_index++] = ' ';
    dest[dest_index++] = ' ';
    for (int i = 0; i < board_size - 1; i++) {
        dest[dest_index++] = '0' + i + 1;
    }
    dest[dest_index++] = '1';
    dest[dest_index++] = '0';
    dest[dest_index++] = '\t';
    dest[dest_index++] = ' ';
    dest[dest_index++] = ' ';
    for (int i = 0; i < board_size - 1; i++) {
        dest[dest_index++] = '0' + i + 1;
    }
    dest[dest_index++] = '1';
    dest[dest_index++] = '0';
    dest[dest_index++] = '\n';

    for (int i = 0; i < board_size + 2; i++) {
        dest[dest_index++] = '_';
    }
    dest[dest_index++] = '\t';
    for (int i = 0; i < board_size + 2; i++) {
        dest[dest_index++] = '_';
    }
    dest[dest_index++] = '\n';

    for (int i = 1; i <= board_size; i++) {
        dest[dest_index++] = 'A' + i - 1;
        dest[dest_index++] = '|';
        for (int j = 1; j <= board_size; j++) {
            char cell_char;
            switch (game->player_board.grid[i][j]) {
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

        dest[dest_index++] = '\t';

        dest[dest_index++] = 'A' + i - 1;
        dest[dest_index++] = '|';
        for (int j = 1; j <= board_size; j++) {
            char cell_char;
            switch (game->server_board.grid[i][j]) {
                case EMPTY:
                    cell_char = '-';
                    break;
                case SHIP:
                    cell_char = '-';
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