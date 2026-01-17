#include "alu_process.h"
#include <climits>
#include <stdio.h>
#include <string.h>

static int_cache_t cache[INT_HASH_SIZE];
static int cache_initialized = 0;
static int replace_idx = 0;

// инициализировать кэш
static void init_cache(void)
{
    for (int i = 0; i < INT_DICT_SIZE; i++)
    {
        cache[i].flag = 0;
        cache[i].key[0] = '\0';
        cache[i].result = 0;
    }
    cache_initialized = 1;
    replace_idx = 0;
}

static void make_key(int operation, long long a, long long b, char *key)
{
    snprintf(key, INT_HASH_SIZE, "%d_%lld_%lld", operation, a, b);
}

static int cache_get(const char *key, long long* result)
{
    if (!cache_initialized)
    {
        init_cache();
    }

    for (int i = 0; i < INT_DICT_SIZE; i++)
    {
        if (cache[i].flag && strcmp(cache[i].key, key) == 0)
        {
            *result = cache[i].result;
            return 1;
        }
    }
    return 0;
}


static void cache_put(const char *key, long long value)
{
    if (!cache_initialized)
    {
        init_cache();
    }

    for (int i = 0; i < INT_DICT_SIZE; i++)
    {
        if (!cache[i].flag)
        {
            strncpy(cache[i].key, key, INT_HASH_SIZE - 1);
            cache[i].key[INT_HASH_SIZE - 1] = '\0';
            cache[i].result = value;
            cache[i].flag = 1;

            return;
        }
    }

    strncpy(cache[replace_idx].key, key, INT_HASH_SIZE - 1);
    cache[replace_idx].key[INT_HASH_SIZE - 1] = '\0';
    cache[replace_idx].result = value;
    cache[replace_idx].flag = 1;
    replace_idx = (replace_idx + 1) % INT_DICT_SIZE;
}

static status_t check_add_overflow(long long a, long long b)
{
    status_t rc = SUCCESS_CODE;

    if (a > 0 && b > 0)
    {
        if (a > INT_MAX - b)
        {
            rc = INT_OVERFLOW;
        }
    }
    else if (rc == SUCCESS_CODE && a < 0 && b < 0)
    {
        if (a < INT_MIN - b)
        {
            rc = INT_OVERFLOW;
        }
    }
    return rc;
}

static status_t check_mult_overflow(long long a, long long b)
{
    if (a > 0 && b > 0)
    {
        if (a > INT_MAX / b)
        {
            return INT_OVERFLOW;
        }
    }
    else if (a < 0 && b < 0)
    {
        if (a < INT_MAX / b)
        {
            return INT_OVERFLOW;
        }
    }
    else if (a > 0 && b < 0)
    {
        if (b < INT_MIN / a)
        {
            return INT_OVERFLOW;
        }
    }
    else if (a < 0 && b > 0)
    {
        if (a < INT_MIN / b)
        {
            return INT_OVERFLOW;
        }
    }
    return SUCCESS_CODE;
}


// сами команды

status_t int_sum(long long a, long long b, long long *result)
{
    char key[INT_HASH_SIZE];
    make_key(1, a, b, key);
    status_t rc = SUCCESS_CODE;

    if (cache_get(key, result))
    {
        rc = SUCCESS_CODE;
    }

    rc = check_add_overflow(a,b);
    if (rc == SUCCESS_CODE)
    {
        *result = a + b;
        cache_put(key, *result);
    }

    return rc;
}


status_t int_sub(long long a, long long b, long long *result)
{
    char key[INT_HASH_SIZE];
    make_key(2, a, b, key);

    if (cache_get(key, result))
    {
        return SUCCESS_CODE;
    }

    status_t rc = check_add_overflow(a, -b);
    if (rc == SUCCESS_CODE)
    {
        *result = a - b;
        cache_put(key, *result);
    }

    return rc;
}

status_t int_mult(long long a, long long b, long long *result)
{
    char key[INT_HASH_SIZE];
    make_key(3, a, b, key);
    status_t rc = SUCCESS_CODE;

    if (cache_get(key, result))
    {
        return rc;
    }

    rc = check_mult_overflow(a, b);
    if (rc == SUCCESS_CODE)
    {
        *result = a * b;
        cache_put(key, *result);
    }

    return SUCCESS_CODE;
}

status_t int_div(long long a, long long b, long long *result)
{
    char key[INT_HASH_SIZE];
    make_key(4, a, b, key);
    status_t rc = SUCCESS_CODE;

    if (cache_get(key, result))
    {
        if (b == 0)
        {
            rc = INT_ZERO_DIVISION;
        }
    }

    if (rc == SUCCESS_CODE)
    {
        *result = a / b;
        cache_put(key, *result);
    }

    return rc;
}

status_t int_cmp(long long a, long long b, long long *result)
{
    char key[INT_HASH_SIZE];

    make_key(5, a, b, key);

    if (cache_get(key, result))
    {
        return SUCCESS_CODE;
    }

    if (a > b)
    {
        *result = 1;
    }
    else if (a == b)
    {
        *result = 0;
    }
    else
    {
        *result = -1;
    }

    cache_put(key, *result);
    return SUCCESS_CODE;
}

status_t int_neg(long long a, long long *result)
{
    char key[INT_HASH_SIZE];

    make_key(6, a, 0, key);

    if (cache_get(key, result))
    {
        return SUCCESS_CODE;
    }

    *result = !a;
    cache_put(key, *result);
    return SUCCESS_CODE;
}
