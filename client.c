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
#include <pthread.h>
#include "sql_queries.h"
#include "messages.h"
#include "utils.h"
#include "game_client.h"

#define MSG_MAX_LEN 4096
#define MULTICAST_GROUP "239.0.0.1"
#define MULTICAST_PORT 12345

void authenticate(int sockfd) {
    int authenticated = 0;
    char message[MSG_MAX_LEN];
    while (!authenticated) {
        char hello[] = "HELLO";
        send_util(sockfd, hello);
        read_util(sockfd, message, MSG_MAX_LEN);
        printf("%s", message);

        char username[USERNAME_LEN + 1];
        scanf("%256s", username);
        send_util(sockfd, username);

        read_util(sockfd, message, MSG_MAX_LEN);
        if (strncmp(message, invalid_username, strlen(invalid_username)) == 0) {
            printf("%s", message);
            continue;
        }

        if (strcmp(message, password_prompt) == 0) { // user exists
            char password[PASSWORD_LEN + 1];
            printf("%s", message);
            scanf("%256s", password);
            send_util(sockfd, password);

            read_util(sockfd, message, MSG_MAX_LEN);
            if (strncmp(message, invalid_password, strlen(invalid_password)) == 0) { // invalid password
                printf("%s", message);
                continue;
            } else if (strncmp(message, auth_success, strlen(auth_success)) == 0) { // password correct
                authenticated = 1;
                send_util(sockfd, "OK");
                continue;
            } else {
                printf("Error, unrecognized message: %s", message);
            }
        } else if (strncmp(message, no_such_user, strlen(no_such_user)) == 0) { // user doesn't exist, register
            printf("%s", message);
            char password[PASSWORD_LEN + 1];
            scanf("%256s", password);
            send_util(sockfd, password);
            read_util(sockfd, message, MSG_MAX_LEN);
            if (strncmp(message, user_added, strlen(user_added)) == 0) { // registered successfully
                printf("%s", message);
                authenticated = 1;
                continue;
            } else if (strncmp(message, user_add_failed, strlen(user_add_failed)) == 0) { // user registration failed
                printf("%s", message);
                continue;
            }
        }
    }
}

void* receive_multicast(void* arg) {
    int multicast_fd;
    struct sockaddr_in multicast_addr;
    struct ip_mreq mreq;
    char msgbuf[MSG_MAX_LEN];
    int nbytes;
    socklen_t addrlen;

    if ((multicast_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    u_int yes = 1;
    if (setsockopt(multicast_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("Reusing ADDR failed");
        exit(1);
    }

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    multicast_addr.sin_port = htons(MULTICAST_PORT);

    if (bind(multicast_fd, (struct sockaddr*) &multicast_addr, sizeof(multicast_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(multicast_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    while (1) {
        addrlen = sizeof(multicast_addr);
        if ((nbytes = recvfrom(multicast_fd, msgbuf, MSG_MAX_LEN, 0, (struct sockaddr*) &multicast_addr, &addrlen)) < 0) {
            perror("recvfrom");
            exit(1);
        }
        msgbuf[nbytes] = '\0';
        printf("Received multicast message: %s\n", msgbuf);
    }

    close(multicast_fd);
    return NULL;
}

int main(int argc, char* argv[]) {
    int                 sockfd, err;
    struct sockaddr_in  servaddr;
    char servip[16] = "172.104.225.148";

    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc > 1) {
        strncpy(servip, argv[1], 16);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "socket error: %s", strerror(errno));
        return 1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1337);

    if ((err = inet_pton(AF_INET, servip, &servaddr.sin_addr) <= 0)) {
        if (err == 0) fprintf(stderr, "inet_pton error for %s", servip);
        else fprintf(stderr, "inet_pton error for %s: %s", servip, strerror(errno));
        return 1;
    }

    if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "connect error: %s", strerror(errno));
        return 1;
    }

    authenticate(sockfd);

    pthread_t multicast_thread;
    if (pthread_create(&multicast_thread, NULL, receive_multicast, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    set_all_ships(sockfd);

    play_game(sockfd);

    pthread_cancel(multicast_thread);
    pthread_join(multicast_thread, NULL);

    close(sockfd);
    return 0;
}
