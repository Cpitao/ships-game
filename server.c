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
#include        "sql_queries.h"


int main(int argc, char* argv[]) {
    
    sqlite3 *db = init_db();
    User admin = {
        .username = "admin",
        .password = "password"
    };
    int rc = add_user(db, &admin);
    if (rc != SQLITE_OK) {
        return 0;   
    }

    User user;
    rc = get_user(db, &user, "admin");
    if (rc != SQLITE_OK) {
        return 0;
    }
    printf("%s %s\n", user.username, user.password);
    printf("OK");

    sqlite3_close(db);
    return 0;
}

