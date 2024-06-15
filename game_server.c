#include "game_server.h"
#include "utils.h"
#include <ctype.h>

void prompt_ship(int fd, Board* board, int size) {
    char *ship_prompt = (char*) malloc(sizeof(char) * 256);
    sprintf(ship_prompt, "Input positions of %d size ship: ", size);
    send_util(fd, ship_prompt);
    char msg[256];
    for (int i=0; i < size; i++) {
        read_util(fd, msg, 2);
        int x, y;
        x = tolower(msg[0]) - 'a';
        y = msg[1] - '0';
        if (x < 0 || x >= 10 || y < 0 || y >= 10) {
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
        board->grid[x][y] = PENDING;
        board_to_string(board, msg);
        printf("%s\n", msg);
        send_util(fd, msg);
    }

    for (int i=0; i < BOARD_SIZE; i++) {
        for (int j=0; j < BOARD_SIZE; j++) {
            if (board->grid[i][j] == PENDING) {
                board->grid[i][j] = SHIP;
            }
        }
    }
    free(ship_prompt);
}

void get_user_ships(int connfd, Board* board) {

    // prompt for ships
    for (int i=0; i < FOUR_SIZE; i++) {
        prompt_ship(connfd, board, 4);
    }
    for (int i=0; i < THREE_SIZE; i++) {
        prompt_ship(connfd, board, 3);
    }
    for (int i=0; i < TWO_SIZE; i++) {
        prompt_ship(connfd, board, 2);
    }
    for (int i=0; i < ONE_SIZE; i++) {
        prompt_ship(connfd, board, 1);
    }
}

void generate_server_board(Board* board) {
    // @TODO
}

void init_game(int connfd, Game* game) {
    Board player_board;
    get_user_ships(connfd, &player_board);
    Board server_board;
    generate_server_board(&server_board);

    game->player_board = player_board;
    game->server_board = server_board;
}