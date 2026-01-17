#ifndef __DB_H__
#define __DB_H__

#include <sqlite3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "errors.h"
#include "cache.h"

#define DATABASE_NAME "app.db"
#define MAX_DB_COMMAND_LENGTH 256

/** @brief Инициализирует базу данных 'app.db' с двумя столбцами: 
 * ключ (строка) - значение (ЧПТ).
 * @return Статус завершения функции.
*/
status_t init_bd(void);

/** @brief Получает пару ключ-значение по ключу key из базы данных.
 * 
 * @param[in] key Ключ, по которому будет осуществляться поиск значения в базе данных.
 * @param[out] value Значение, полученное по ключу.
 * 
 * @return Статус завершения функции.
*/
status_t get_element_from_bd(const char *key, double *value);

/** @brief Добавляет новые данные (пару ключ-значение) в базу данных.
 * 
 * @param[in] key Ключ, по которому будет добавлено (обновлено) значение в базе данных.
 * @param[in] value Значение, которое будет добавлено в базу по данному ключу key.
 * 
 * @return Статус завершения функции.
*/
status_t insert_elem_to_bd(const char *key, double value);


#endif