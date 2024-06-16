#include "game_server.h"
#include "utils.h"
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

int valid_position_for_ship(Board* board, int x, int y) {
    if (x > BOARD_SIZE || x < 1 || y > BOARD_SIZE || y < 1) return 0;
    if (board->grid[x][y] == SHIP || board->grid[x][y] == PENDING) return 0;
    int shifts[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}; 
    for (int s=0; s < 8; s++) {
        if (board->grid[x - shifts[s][0]][y - shifts[s][1]] == SHIP) {
            return 0;
        }
    }
    return 1;
}

void prompt_ship(int fd, Board* board, int size) {
    char *ship_prompt = (char*) malloc(sizeof(char) * 256);
    sprintf(ship_prompt, "Input positions of %d size ship: ", size);
    send_util(fd, ship_prompt);
    char msg[256];
    for (int i=0; i < size; i++) {
        read_util(fd, msg, 255);
        int x, y;
        x = tolower(msg[0]) - 'a' + 1;
        if (strlen(msg) >= 2 && msg[1] == '1' && msg[2] == '0') y = 10;
        else y = msg[1] - '0';

        if (x <= 0 || x > 10 || y <= 0 || y > 10) {
            i--;
            send_util(fd, "Invalid position\n");
            continue;
        }
        if (i > 0 && (board->grid[x-1][y] != PENDING && 
            board->grid[x+1][y] != PENDING &&
            board->grid[x][y-1] != PENDING &&
            board->grid[x][y+1] != PENDING)) {
            i--;
            send_util(fd, "Invalid position\n");
            continue;
        }
        if (!valid_position_for_ship(board, x, y)) {
            i--;
            send_util(fd, "Invalid position\n");
            continue;
        }
        board->grid[x][y] = PENDING;
        board_to_string(board, msg);
        send_util(fd, msg);
    }

    for (int i=1; i < BOARD_SIZE+1; i++) {
        for (int j=1; j < BOARD_SIZE+1; j++) {
            if (board->grid[i][j] == PENDING) {
                board->grid[i][j] = SHIP;
            }
        }
    }
    read_util(fd, msg, 255);
    free(ship_prompt);
}

void get_user_ships(int connfd, Board* board) {

    // prompt for ships
    for (int i=0; i < FOUR_SIZE; i++) {
        prompt_ship(connfd, board, 4);
    }
//    for (int i=0; i < THREE_SIZE; i++) {
//        prompt_ship(connfd, board, 3);
//    }
//    for (int i=0; i < TWO_SIZE; i++) {
//        prompt_ship(connfd, board, 2);
//    }
//    for (int i=0; i < ONE_SIZE; i++) {
//        prompt_ship(connfd, board, 1);
//    }
}

void generate_ship(Board* board, int size) {
    int shifts[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    srand(time(NULL));
    int x = rand() % BOARD_SIZE + 1;
    int y = rand() % BOARD_SIZE + 1;
    while (!valid_position_for_ship(board, x, y)) {
        x = rand() % BOARD_SIZE + 1;
        y = rand() % BOARD_SIZE + 1;
    }
    board->grid[x][y] = PENDING;

    for (int i=0; i < size - 1; i++) {
        int shift = rand() % 4;
        int valid = 0;
        for (int j=0; j < 4; j++) {
            if (!valid_position_for_ship(board, x + shifts[shift][0], y + shifts[shift][1])) {
                shift = (shift + 1) % 4;
            } else {
                valid = 1;
                break;
            }
        }
        if (!valid) {
            for (int k=1; k <= BOARD_SIZE; k++) {
                for (int l=1; l <= BOARD_SIZE; l++) {
                    if (board->grid[k][l] == PENDING) board->grid[k][l] = EMPTY;
                }
            }
            return generate_ship(board, size);
        }
        x += shifts[shift][0];
        y += shifts[shift][1];
        board->grid[x][y] = PENDING;
    }
    for (int i=1; i < BOARD_SIZE + 1; i++) {
        for (int j=1; j < BOARD_SIZE + 1; j++) {
            if (board->grid[i][j] == PENDING) {
                board->grid[i][j] = SHIP;
            }
        }
    }
}

void generate_server_board(Board* board) {
    for (int i=0; i < FOUR_SIZE; i++) {
        generate_ship(board, 4);
    }
    for (int i=0; i < THREE_SIZE; i++) {
        generate_ship(board, 3);
    }
    for (int i=0; i < TWO_SIZE; i++) {
        generate_ship(board, 2);
    }
    for (int i=0; i < ONE_SIZE; i++) {
        generate_ship(board, 1);
    }
}

void init_game(int connfd, Game* game) {
    Board player_board, server_board;
    for (int i=0; i <= BOARD_SIZE + 1; i++) {
        for (int j=0; j <= BOARD_SIZE + 1; j++) {
            player_board.grid[i][j] = EMPTY;
            server_board.grid[i][j] = EMPTY;
        }
    }
    get_user_ships(connfd, &player_board);
    generate_server_board(&server_board);

    game->player_board = player_board;
    game->server_board = server_board;
    srand(time(NULL));
    game->move = rand() % 2;
}

/* return 1 if player wins, 0 on lose */
int play_game(int fd, Game* game) {
    srand(time(NULL));
    sleep(1);
    return rand() % 2;
    char msg[2048];
    while(1) {
        if (game->move == SERVER_MOVE) {
            int x = rand() % BOARD_SIZE + 1;
            int y = rand() % BOARD_SIZE + 1;
            if (game->player_board.grid[x][y] == HIT ||
                game->player_board.grid[x][y] == MISS) continue;
            if (shoot(&(game->player_board), x, y) == HIT_MOVE) {
                char game_string[1024], header[64];
                sprintf(header, "Server hit at %c%d", 'A' + x - 1, y);
                game_to_string(game, game_string);
                sprintf(msg, "%s\n%s", header, game_string);
                send_util(fd, msg);
            } else {
                char game_string[1024], header[64];
                sprintf(header, "Server miss at %c%d", 'A' + x - 1, y);
                game->move = CLIENT_MOVE;
                game_to_string(game, game_string);
                sprintf(msg, "%s\n%s", header, game_string);
                send_util(fd, msg);
            }
            read_util(fd, msg, 2);
            if (check_win(&(game->player_board))) {
                send_util(fd, "Server wins!\n");
                return 0;
            }
        } else {
            char your_move[] = "Your move: ";
            send_util(fd, your_move);
            read_util(fd, msg, 255);
            int x, y;
            x = tolower(msg[0]) - 'a' + 1;
            if (strlen(msg) >= 2 && msg[1] == '1' && msg[2] == '0') y = 10;
            else y = msg[1] - '0';

            if (x <= 0 || x > 10 || y <= 0 || y > 10 || game->server_board.grid[x][y] == HIT || game->server_board.grid[x][y] == MISS) {
                send_util(fd, "Invalid move\n");
                read_util(fd, msg, 3);
                continue;
            }

            if (shoot(&(game->server_board), x, y) == HIT_MOVE) {
                char game_string[1024], header[64];
                sprintf(header, "You hit at %c%d", 'A' + x - 1, y);
                game_to_string(game, game_string);
                sprintf(msg, "%s\n%s", header, game_string);
                send_util(fd, msg);
            } else {
                char game_string[1024], header[64];
                sprintf(header, "You miss at %c%d", 'A' + x - 1, y);
                game->move = SERVER_MOVE;
                game_to_string(game, game_string);
                sprintf(msg, "%s\n%s", header, game_string);
                send_util(fd, msg);
            }
            read_util(fd, msg, 2);
            if (check_win(&(game->server_board))) {
                send_util(fd, "You win!\n");
                return 1;
            }
        }
    }
}
