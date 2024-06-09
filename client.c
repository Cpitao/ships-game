#include        <sys/types.h>
#include        <sys/socket.h>
#include        <sys/time.h>
#include        <time.h>
#include        <netinet/in.h>
#include        <arpa/inet.h>
#include        <errno.h>
#include        <fcntl.h>
#include        <netdb.h>
#include        <signal.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <unistd.h>
#include        <sys/wait.h>
#include        "sql_queries.h"
#include        "client_handler.h"

int main(int argc, char* argv[]) {

    int                 sockfd, err;
    struct sockaddr_in  servaddr;
    char servip[] = "172.104.225.148";
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return 1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1337);

    if ((err = inet_pton(AF_INET, servip, &servaddr.sin_addr) <= 0)) {
        if (err == 0) fprintf(stderr, "inet_pton error for %s\n", servip);
        else fprintf(stderr, "inet_pton error for %s: %s\n", servip, strerror(errno));
        return 1;
    }

    if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "connect error: %s\n", strerror(errno));
        return 1;
    }

    close(sockfd);
}