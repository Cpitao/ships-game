#include "client_handler.h"

void authenticate(int connfd, sqlite3 *db, User *user) {
    int n, status;
    int authenticated = 0;

    while (!authenticated) {
        char hello[256];
        read_util(connfd, hello, 256);
        send_util(connfd, username_prompt);

        char username[USERNAME_LEN + 1];
        n = read_util(connfd, username, USERNAME_LEN);
        if (n <= 0) {
            send_util(connfd, invalid_username);
            continue;
        }

        if ((status = get_user(db, user, username)) == SQLITE_OK) {
            send_util(connfd, password_prompt);

            char password[PASSWORD_LEN + 1];
            n = read_util(connfd, password, PASSWORD_LEN);
            
            if (strcmp(password, user->password) != 0) {
                send_util(connfd, invalid_password);
                continue;
            }

            authenticated = 1;
            send_util(connfd, auth_success);
            char msg[2];
            read_util(connfd, msg, 2);
        } else if (status == SQLITE_DONE) {
            send_util(connfd, no_such_user);

            char password[PASSWORD_LEN + 1];
            n = read_util(connfd, password, PASSWORD_LEN);
            if (n >= 0) {
                strncpy(user->username, username, USERNAME_LEN + 1);
                strncpy(user->password, password, PASSWORD_LEN + 1);
                if ((status = add_user(db, user)) == SQLITE_OK) {
                    send_util(connfd, user_added);
                } else {
                    send_util(connfd, user_add_failed);
                }
            }

            authenticated = 1;
        }
    }
}



void handle_client(sqlite3 *db, int connfd) {
    User user;

    authenticate(connfd, db, &user);
    printf("%s auth OK\n", user.username);
    Game game;
    init_game(connfd, &game);
}