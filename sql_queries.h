#include        <sqlite3.h>

typedef struct {
    int id;
    char username[256];
    char password[256];
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
