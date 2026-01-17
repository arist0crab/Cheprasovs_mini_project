#include <sqlite3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "db.h"

static status_t find_in_cache(cache_t *cache, const char *key, ssize_t *index);
static status_t dynamic_strcpy(const char *src, char **dest);

// callback-функции
static int get_value_callback(void *data, int argc, char **argv, char **col_names);
static int check_found_callback(void *data, int argc, char **argv, char **col_names);

// =========== РАБОТА С БАЗОЙ ДАННЫХ ===========

status_t init_bd(void)
{
    sqlite3 *db = NULL;
    char *err_msg = NULL;
    int sql_rc = 0;

    const char *sql_create_command = 
    "CREATE TABLE IF NOT EXISTS app_db ("
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
    bool found = false;

    char sql_command_get[MAX_DB_COMMAND_LENGTH];
    
    if (key == NULL || value == NULL)
        rc = ERR_ARGS;

    if (rc == SUCCESS_CODE)
        sql_rc = sqlite3_open(DATABASE_NAME, &db);

    if (rc == SUCCESS_CODE && sql_rc == SQLITE_OK)
    {
        *value = 0;
        snprintf(sql_command_get, sizeof(sql_command_get), "SELECT value FROM app_db WHERE key = '%s';", key);
        sql_rc = sqlite3_exec(db, sql_command_get, check_found_callback, value, &err_msg);
    }

    if (rc == SUCCESS_CODE && sql_rc == SQLITE_OK)
        sql_rc = sqlite3_exec(db, sql_command_get, get_value_callback, value, &err_msg);

    if (db) sqlite3_close(db);

    if (found == false) rc = ERR_NOT_FOUND;

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

static int get_value_callback(void *data, int argc, char **argv, char **col_names)
{
    double *value_ptr = (double *)data;

    if (argc > 0 && argv[0] != NULL)
        *value_ptr = atof(argv[0]);
    
    return 0;
}

static int check_found_callback(void *data, int argc, char **argv, char **col_names)
{
    int *found_ptr = (int*)data;
    *found_ptr = 1; 

    return 0;
}

// =============== РАБОТА С КЭШЕМ ================

status_t init_cache(cache_t *cache)
{
    if (cache)
    {
        for (size_t i = 0; i < CACHE_ARRAY_SIZE; i++)
        {
            cache->items[i].key[0] = '\0';
            cache->items.value = 0;
        }
        cache->current_size = 0;
    }

    return (cache) ? SUCCESS_CODE : ERR_ARGS;
}

status_t remove_from_cache(cache_t *cache, size_t index)
{
    status_t rc = SUCCESS_CODE;

    if (cache == NULL || index > cache->current_size - 1) rc = ERR_ARGS;

    if (rc == SUCCESS_CODE && cache->current_size)
    {
        free(cache->items[index].key);
        cache->items[index].key = NULL;

        if (index < cache->current_size - 1) 
        {
            for (size_t i = index; i < cache->current_size - 1; i++) 
            {
                free(cache->items[i].key);
                cache->items[i].key = cache->items[i + 1].key;
                cache->items[i].value = cache->items[i + 1].value;
            }
        }

        cache->current_size--;
        if (cache->current_size >= 0 && cache->current_size < CACHE_SIZE) 
        {
            cache->items[cache->current_size].key = NULL;
            cache->items[cache->current_size].value = 0.0;
        }
    }

    return rc;
}

status_t insert_to_cache(cache_t *cache, const char *key, const double value)
{
    status_t rc = SUCCESS_CODE;
    ssize_t key_index = -1;

    if (cache == NULL || key == NULL) rc = ERR_ARGS;

    if (rc == SUCCESS_CODE)
    {
        find_in_cache(cache, key, &key_index);
        if (key_index >= 0)
            cache->items[key_index].value = value;
        else
        {   
            if (cache->current_size == CACHE_ARRAY_SIZE)
                insert_elem_to_bd(cache->items[0].key, cache->items[0].value);
                remove_from_cache(cache, 0);
            dynamic_strcpy(key, &cache->items[cache->current_size].key);
            cache->items[cache->current_size].value = value;
            cache->current_size++;
        }
    }

    return rc;
}

status_t flush_cash(cache_t *cache)
{
    status_t rc = SUCCESS_CODE;

    if (cache == NULL) rc = ERR_ARGS;

    for (size_t i = 0; rc == SUCCESS_CODE && i < cache->current_size; i++)
        insert_elem_to_bd(cache->items.key, cache->items->value);

    return rc;
}

static status_t find_in_cache(cache_t *cache, const char *key, ssize_t *index)
{
    status_t rc = SUCCESS_CODE;

    if (key == NULL || index == NULL)
        rc = ERR_ARGS;

    if (rc == SUCCESS_CODE)
    {   
        index = -1;
        for (ssize_t i = 0; i < cache->current_size; i++)
            if(strncmp(key, cache->items[i].key, sizeof(key)) == 0)
                index = i;
    }

    return rc;
}

static status_t dynamic_strcpy(const char *src, char **dest)
{
    status_t rc = SUCCESS_CODE;
    
    if (src == NULL || *dest == NULL) rc = ERR_ARGS;

    if (rc == SUCCESS_CODE)
    {
        size_t len = strlen(src) + 1;
        *dest = (char *)malloc(len);
        if (*dest) strcpy(dest, src);
        else rc = ERR_MEM;
    }
    
    return rc;
}