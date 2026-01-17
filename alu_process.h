#ifndef ALU_PROCESS_H
#define ALU_PROCESS_H

#include "errors.h"

/*** + - * / >= <= логическое не
 * не забыть про код ошибки
 * команда 1: сложить два int
 * команда 2: int_1 - int_2
 * команда 3: int_1 * int_2
 * команда 4: int_1 / int_2
 * команда 5: сравниваем int_1, int_2; если int_1 > int_2 (возврат 1), если равно (возврат 0), если int_1 < int_2 (возврат -1)
 * команда 6: логическое отрицание числа int_1
 *
 * еще есть кэширование: таблица ключ-значение, хэш-функция, кол-во строк у таблицы максимум 5;
 * если результат команды (хэш) уже существует, то не вычисляем операцию, а возвращаем из таблички значение
 ***/

#define INT_DICT_SIZE 5
#define INT_HASH_SIZE 256

typedef struct
{
    char key[INT_HASH_SIZE]; // ключ
    long long result; // значение
    int flag; // занято или нет
} int_cache_t;

status_t int_sum(long long a, long long b, long long *result);
status_t int_sub(long long a, long long b, long long *result);
status_t int_mult(long long a, long long b, long long *result);
status_t int_div(long long a, long long b, long long *result);
status_t int_cmp(long long a, long long b, long long *result);
status_t int_neg(long long a, long long *result);

#endif
