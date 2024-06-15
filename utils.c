#include "utils.h"

void send_util(int fd, const char *message) {
    if (send(fd, message, strlen(message), 0) < 0) {
        perror("send error");
        exit(1);
    }
}

int read_util(int fd, char* dest, size_t count) {
    int n;
    if ((n = read(fd, dest, count)) == -1) {
        perror("read error");
        exit(1);
    }
    dest[n] = '\0';
    return n;
}