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
#include        <netinet/tcp.h>
#include        "client_handler.h"

#ifndef SQL_QUERIES
#include        "sql_queries.h"
#endif

#define LISTENQ 100

int running = 1;

void stop_server(int signum) {
    printf("SIGINT received. Preparing to shutdown..."); fflush(stdout);
    running = 0;
}

int run_server(sqlite3 *db) {
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    int childpid;
    socklen_t len;

    signal(SIGINT, stop_server);
    
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return 1;
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        fprintf(stderr, "setsockopt error: %s\n", strerror(errno));
        return 1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1337);

    if (bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "bind error: %s\n", strerror(errno));
        return 1;
    }

    if (listen(listenfd, LISTENQ) < 0) {
        fprintf(stderr, "listen error: %s\n", strerror(errno));
        return 1;
    }

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    fprintf(stdout, "Waiting for connections ... \n");
    while (running) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(listenfd, &fds);

        len = sizeof(cliaddr);

        int retval = select(listenfd + 1, &fds, NULL, NULL, &tv);
        if (retval == -1) {
            fprintf(stderr, "select error: %s\n", strerror(errno));
            continue;
        } else if (retval == 0) continue;
        
        if ((connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &len)) < 0) {
            fprintf(stderr, "accept error: %s\n", strerror(errno));
            continue;
        }
        // setsockopt(connfd, IPPROTO_TCP, TCP_NODELAY, &(char){1}, sizeof(int));

        if ((childpid = fork()) == 0) {
            close(listenfd);
            handle_client(db, connfd);
            exit(0);
        }
        close(connfd);
    }

    int pid;
    while ((pid = wait(NULL)) > 0) {
        fprintf(stdout, "Child %d finished", pid);
    }

    printf("Closing...");
    return 0;
}

int main(int argc, char* argv[]) {

    setvbuf(stdout, NULL, _IONBF, 0);
    
    sqlite3 *db = init_db();
    
    run_server(db);

    sqlite3_close(db);
    return 0;
}

