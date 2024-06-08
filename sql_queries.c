#include "sql_queries.h"
#include <stdio.h>
#include <string.h>

const char* SQL_CREATE_USERS_TABLE = 
    "CREATE TABLE IF NOT EXISTS users ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "username VARCHAR(256) UNIQUE NOT NULL,"
    "password VARCHAR(256) NOT NULL"
    ");";
const char* SQL_CREATE_SCORES_TABLE =
    "CREATE TABLE IF NOT EXISTS scores ("
    "user_id INTEGER PRIMARY KEY,"
    "wins INTEGER NOT NULL DEFAULT 0,"
    "loses INTEGER NOT NULL DEFAULT 0,"
    "FOREIGN KEY(user_id) REFERENCES users(id)"
    ");";
const char *SQL_SELECT_USER = 
    "SELECT id, username, password FROM users WHERE username = ?";
const char* SQL_ADD_USER = 
    "INSERT INTO users (username, password) VALUES (?, ?)";
const char* SQL_UPDATE_SCORE = "";
const char* SQL_GET_SCORE = "";

int execute_sql(sqlite3 *db, const char *sql) {
    char* err;
    int rc = sqlite3_exec(db, sql, 0, 0, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err);
        sqlite3_free(err);
        return rc;
    }
    return SQLITE_OK;
}

sqlite3* init_db() {
    sqlite3 *db;

    int rc = sqlite3_open("sqlite.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return NULL;
    } else {
        fprintf(stdout, "DB \xe2\x9c\x93\n");
    }   

    rc = execute_sql(db, SQL_CREATE_USERS_TABLE);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return NULL;
    }
    fprintf(stdout, "Table users \xe2\x9c\x93\n");

    rc = execute_sql(db, SQL_CREATE_SCORES_TABLE);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return NULL;
    }
    fprintf(stdout, "Table scores \xe2\x9c\x93\n");
    return db;
}

int get_user(sqlite3 *db, User *user, const char *username) {
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, SQL_SELECT_USER, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statment: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind parameter: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        user->id = sqlite3_column_int(stmt, 0);
        const unsigned char *db_username = sqlite3_column_text(stmt, 1);
        const unsigned char *db_password = sqlite3_column_text(stmt, 2);

        snprintf(user->username, sizeof(user->username), "%s", db_username);
        snprintf(user->password, sizeof(user->password), "%s", db_password);
    } else if (rc == SQLITE_DONE) {
        printf("No such user.\n");
        sqlite3_finalize(stmt);
        return SQLITE_DONE;
    } else {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int add_user(sqlite3 *db, const User *user) {
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, SQL_ADD_USER, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 1, user->username, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind username parameter: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 2, user->password, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind password parameter: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}