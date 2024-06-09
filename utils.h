#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void send_util(int fd, const char *message);
int read_util(int fd, char* dest, size_t count);