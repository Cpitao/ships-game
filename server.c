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

#define LISTENQ 100

int running = 1;

void stop_server(int signum) {
    printf("SIGINT received. Preparing to shutdown...");
    running = 0;
}

int run_server() {
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    int childpid;
    socklen_t len;
    
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

    fprintf(stdout, "Waiting for connections ... \n");
    while (running) {
        len = sizeof(cliaddr);
        if ((connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &len)) < 0) {
            fprintf(stderr, "accept error: %s\n", strerror(errno));
            continue;
        }

        if ((childpid = fork()) == 0) {
            close(listenfd);
            handle_client(connfd);
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
    
    sqlite3 *db = init_db();
    
    run_server();

    sqlite3_close(db);
    return 0;
}

