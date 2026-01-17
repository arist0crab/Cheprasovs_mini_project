#include <sqlite3.h>
#include <stdio.h>
#include <stdbool.h>
#include "errors.h"
#include "db.h"

static int callback(void *data, int argc, char **argv, char **col_names);

status_t init_bd(void)
{
    sqlite3 *db = NULL;
    char *err_msg = NULL;
    int sql_rc = 0;

    // NOTE можнос сделать так, чтобы БД не сбрасывалась каждый раз
    const char *sql_create_command = 
    "DROP TABLE IF EXISTS app_db;"
    "CREATE TABLE app_db ("
    "    key TEXT PRIMARY KEY,"
    "    value REAL NOT NULL"
    ");";

    sql_rc = sqlite3_open(DATABASE_NAME, &db);

    if (sql_rc != SQLITE_OK) sqlite3_close(db);
    else sql_rc = sqlite3_exec(db, sql_create_command, 0, 0, &err_msg);

    if (db) sqlite3_close(db);
    if (sql_rc != SQLITE_OK) sqlite3_free(err_msg);

    return (sql_rc == SQLITE_OK) ? SUCCESS_CODE : ERR_DB;
}

status_t get_element_from_bd(const char *key, double *value)
{
    status_t rc = SUCCESS_CODE;
    sqlite3 *db = NULL;
    char *err_msg = NULL;
    int sql_rc = 0;

    // TODO добавить обработку случая, когда не было найдено в БД

    char sql_command_get[MAX_DB_COMMAND_LENGTH];
    
    if (key == NULL || value == NULL)
        rc = ERR_ARGS;

    if (rc == SUCCESS_CODE)
        sql_rc = sqlite3_open(DATABASE_NAME, &db);

    if (rc == SUCCESS_CODE && sql_rc == SQLITE_OK)
    {
        *value = 0;
        snprintf(sql_command_get, sizeof(sql_command_get), "SELECT value FROM app_db WHERE key = '%s';", key);
        sql_rc = sqlite3_exec(db, sql_command_get, callback, value, &err_msg);
    }

    if (db) sqlite3_close(db);

    if (sql_rc != SQLITE_OK && rc == SUCCESS_CODE) rc = ERR_DB;
    if (sql_rc != SQLITE_OK) sqlite3_free(err_msg);

    return rc;
}

status_t insert_elem_to_bd(const char *key, double value)
{
    status_t rc = SUCCESS_CODE;
    sqlite3 *db = NULL;
    char *err_msg = 0;
    int sql_rc = 0;

    char *sql_command_insert[MAX_DB_COMMAND_LENGTH];

    if (key == NULL || value == NULL)
        rc = ERR_ARGS;

    if (rc == SUCCESS_CODE)
        sql_rc = sqlite3_open(DATABASE_NAME, &db);

    if (rc == SUCCESS_CODE && sql_rc == SQLITE_OK)
    {
        snprintf(sql_command_insert, sizeof(sql_command_insert), "INSERT OR REPLACE INTO app_db (key, value) VALUES ('%s', %lf);", key, value);
        sql_rc = sqlite3_exec(db, sql_command_insert, NULL, NULL, &err_msg);
    }

    if (db) sqlite3_close(db);

    if (sql_rc != SQLITE_OK && rc == SUCCESS_CODE) rc = ERR_DB;
    if (sql_rc != SQLITE_OK) sqlite3_free(err_msg);

    return rc;
}

static int callback(void *data, int argc, char **argv, char **col_names)
{
    double *value_ptr = (double *)data;
    
    if (argc > 0 && argv[0] != NULL)
        *value_ptr = atof(argv[0]);
    
    return 0;
}