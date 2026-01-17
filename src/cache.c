#include "cache.h"

static status_t dynamic_strcpy(const char *src, char **dest);

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
        if (cache->current_size < CACHE_ARRAY_SIZE) 
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
            {
                insert_elem_to_bd(cache->items[0].key, cache->items[0].value);
                remove_from_cache(cache, 0);
            }
            dynamic_strcpy(key, &cache->items[cache->current_size].key);
            cache->items[cache->current_size].value = value;
            cache->current_size++;
        }
    }

    return rc;
}

status_t flush_cache(cache_t *cache)
{
    status_t rc = SUCCESS_CODE;

    if (cache == NULL) rc = ERR_ARGS;

    for (size_t i = 0; rc == SUCCESS_CODE && i < cache->current_size; i++)
        insert_elem_to_bd(cache->items[i].key, cache->items[i].value);

    return rc;
}

status_t find_in_cache(cache_t *cache, const char *key, ssize_t *index)
{
    status_t rc = SUCCESS_CODE;

    if (key == NULL || index == NULL)
        rc = ERR_ARGS;

    if (rc == SUCCESS_CODE)
    {   
        *index = -1;
        for (ssize_t i = 0; i < (ssize_t)cache->current_size; i++)
            if(strncmp(key, cache->items[i].key, strlen(key)) == 0)
                *index = i;
    }

    return rc;
}

static status_t dynamic_strcpy(const char *src, char **dest)
{
    status_t rc = SUCCESS_CODE;
    
    if (src == NULL || dest == NULL) rc = ERR_ARGS;

    if (rc == SUCCESS_CODE)
    {
        size_t len = strlen(src) + 1;
        *dest = (char *)malloc(len);
        if (*dest) strcpy(*dest, src);
        else rc = ERR_MEM;
    }
    
    return rc;
}