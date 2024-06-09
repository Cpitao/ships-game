#include <sys/socket.h>
#include <string.h>

void send_util(int fd, const char *message);
int read_util(int fd, char* dest, size_t count);