#include        <sqlite3.h>

#define SQL_QUERIES 1

#define USERNAME_LEN 256
#define PASSWORD_LEN 256

typedef struct {
    int id;
    char username[USERNAME_LEN + 1];
    char password[PASSWORD_LEN + 1];
} User;

typedef struct {
    int user_id;
    int wins;
    int loses;
} Score;

sqlite3* init_db();
int execute_sql(sqlite3 *db, const char *sql);
int get_user(sqlite3 *db, User *user, const char *username);
int add_user(sqlite3 *db, const User *user);
int add_win(sqlite3* db, const User* user);
int add_loss(sqlite3* db, const User* user);
