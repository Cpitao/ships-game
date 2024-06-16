#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netinet/tcp.h>
#include "client_handler.h"

#ifndef SQL_QUERIES
#include        "sql_queries.h"
#endif

#define LISTENQ 100
#define MULTICAST_GROUP "239.0.0.1"
#define MULTICAST_PORT 12345

int running = 1;

void stop_server(int signum) {
    printf("SIGINT received. Preparing to shutdown..."); fflush(stdout);
    running = 0;
}

int send_multicast_message(const char* message) {
    int multicast_fd;
    struct sockaddr_in multicast_addr;

    if ((multicast_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return -1;
    }

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    multicast_addr.sin_port = htons(MULTICAST_PORT);

    if (sendto(multicast_fd, message, strlen(message), 0, (struct sockaddr*) &multicast_addr, sizeof(multicast_addr)) < 0) {
        fprintf(stderr, "sendto error: %s\n", strerror(errno));
        close(multicast_fd);
        return -1;
    }

    close(multicast_fd);
    return 0;
}

void update_scoreboard(sqlite3 *db) {
    char scoreboard[1024] = "dupa dupa multicast\n";
//    prepare_scoreboard(db, scoreboard, sizeof(scoreboard));

    if (send_multicast_message(scoreboard) < 0) {
        fprintf(stderr, "Error sending multicast message\n");
    }
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
            int flag = 1;
            if (setsockopt(connfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(int)) < 0) {
                perror("setsockopt");
                close(connfd);
                exit(EXIT_FAILURE);
            }
            handle_client(db, connfd);
            update_scoreboard(db);
            exit(0);
        }
        close(connfd);
    }

    int pid;
    while ((pid = wait(NULL)) > 0) {
        fprintf(stdout, "Child %d finished\n", pid);
    }

    printf("Closing...");
    return 0;
}

int main(int argc, char* argv[]) {

    setvbuf(stdout, NULL, _IONBF, 0);
    
    sqlite3 *db = init_db();
    if (db == NULL) {
        fprintf(stderr, "Error initializing database\n");
        return 1;
    }
    run_server(db);

    sqlite3_close(db);
    return 0;
}

