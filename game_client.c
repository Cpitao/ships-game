#include "game_client.h"
#include "utils.h"

void set_ship(int fd, int size) {
    char *msg = (char*) malloc(sizeof(char) * 256);
    read_util(fd, msg, 255);
    printf("%s", msg);

    for (int i=0; i < size; i++) {
        scanf("%255s", msg);
        send_util(fd, msg);
        read_util(fd, msg, 255);
        if (strcmp(msg, "Invalid position\n") == 0) {
            i--;
        }
        printf("%s\n", msg);
    }

    send_util(fd, "OK");

    return;
}

void set_all_ships(int fd) {
    for (int i=0; i < FOUR_SIZE; i++) {
        set_ship(fd, 4);
    }
    for (int i=0; i < THREE_SIZE; i++) {
        set_ship(fd, 3);
    }
    for (int i=0; i < TWO_SIZE; i++) {
        set_ship(fd, 2);
    }
    for (int i=0; i < ONE_SIZE; i++) {
        set_ship(fd, 1);
    }
}

void play_game(int fd) {
    char msg[2048];
    while (1) {
        read_util(fd, msg, 2047);
        printf("%s", msg);
        if (strcmp(msg, "Your move: ") == 0) {
            scanf("%s", msg);
            send_util(fd, msg);
            read_util(fd, msg, 2047);
            printf("%s", msg);
            send_util(fd, "OK");
        } else if (strcmp(msg, "Server wins!\n") == 0 ||
            strcmp(msg, "You win!\n") == 0) {
            return;
        } else {
            send_util(fd, "OK");
        }
    }
}