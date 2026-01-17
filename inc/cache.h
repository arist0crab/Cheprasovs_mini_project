#ifndef __CACHE_H__
#define __CACHE_H__

#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <db.h>
#include "errors.h"

#define CACHE_ARRAY_SIZE 5

typedef struct 
{
    char *key;               
    double value;  
} cache_node_t;

typedef struct 
{
    cache_node_t items[CACHE_ARRAY_SIZE]; 
    size_t current_size;                      
} cache_t;

/** @brief Функция для вставки пары ключ-значение в кэш базы данных.
 * 
 * @param[in, out] cache Указатель на структуру кэша, в которую добавляются данные.
 * @param[in] key Ключ-строка, которая будет добавлена в кэш.
 * @param[in] value Значение по ключу, которое будет добавлено в базу данных. 
 * 
 * @return Статус завершения функции.
*/
status_t insert_to_cache(cache_t *cache, const char *key, const double value);

/** @brief Удаляет последние данные из кэша.
 * 
 * @param[in, out] cache Указатель на структуру кэша, из которой удаляются данные.
 * @param[in] index Индекс данных (от 0 до CACHE_ARRAY_SIZE - 1), которые необходимо удалить.
 * 
 * @return Статус завершения функции.
*/
status_t remove_from_cache(cache_t *cache, size_t index);

/** @brief Выгружает все данные из кэша в базу данных.
 * @return Статус завершения функции.
*/
status_t flush_cache(cache_t *cache);

/** @brief Ищет данные в кеше по ключу key. Индекс нужных данных записывается в index.
 * 
 * @param cache Указатель на структуру кэша, в которой будет происходить поиск данных.
 * @param key Ключ-строка, по которой будет происходить поиск в кэше.
 * @param index Указатель на индекс найденных данных. Если совпадений по ключу не было найденно,
 * то index = -1 (тип ssize_t).
 * 
 *  @return Статус завершения функции.
*/
status_t find_in_cache(cache_t *cache, const char *key, ssize_t *index);

#endif 