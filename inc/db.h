#ifndef __DB_H__
#define __DB_H__

#define DATABASE_NAME "app.db"
#define MAX_DB_COMMAND_LENGTH 256
#define CACHE_ARRAY_SIZE 5

typedef struct 
{
    char *key;               
    double value;  
} cache_node_t;

typedef struct 
{
    cache_node_t items[CACHE_SIZE]; 
    int current_size;                      
} cache_t;

#endif