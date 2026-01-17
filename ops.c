#include "ops.h"

fpu_entry_t fpu_cache[FPU_CACHE_SIZE];
size_t fpu_cache_index = 0;

static status_t fpu_cache_search(fpu_op_t op, double a, double b, double *result)
{
    bool entry_found = false;

    for (size_t i = 0; (i < FPU_CACHE_SIZE) && (!entry_found); i++)
    {
        if (fpu_cache[i].is_valid &&
            fpu_cache[i].key.fpu_op == op &&
            (fabs(fpu_cache[i].key.a - a) < EPS) &&
            (fabs(fpu_cache[i].key.b - b) < EPS))
        {
            *result = fpu_cache[i].result;
            entry_found = true;
        }
    }
    return entry_found;
}

static void fpu_cache_store(fpu_op_t op, double a, double b, double result) {
    fpu_entry_t *entry = &fpu_cache[fpu_cache_index];

    entry->is_valid = 1;
    entry->key.fpu_op = op;
    entry->key.a = a;
    entry->key.b = b;
    entry->result = result;

    fpu_cache_index = (fpu_cache_index + 1) % FPU_CACHE_SIZE;
}

status_t fpu_add(double a, double b, double *result) 
{
    status_t exit_code = SUCCESS_CODE;

    if (!fpu_cache_search(FPU_OP_ADD, a, b, result))
    {
        *result = a + b;
        fpu_cache_store(FPU_OP_ADD, a, b, *result);
    }
    return exit_code;
}

status_t fpu_sub(double a, double b, double *result) 
{
    status_t exit_code = SUCCESS_CODE;

    if (!fpu_cache_search(FPU_OP_SUB, a, b, result))
    {
        *result = a - b;
        fpu_cache_store(FPU_OP_SUB, a, b, *result);
    }
    return exit_code;
}

status_t fpu_div(double a, double b, double *result) 
{
    status_t exit_code = SUCCESS_CODE;

    if (fabs(b) < EPS)
        exit_code = ERR_RANGE;

    else if (!fpu_cache_search(FPU_OP_DIV, a, b, result))
    {
        *result = a / b;
        fpu_cache_store(FPU_OP_DIV, a, b, *result);
    }
    return exit_code;
}

status_t fpu_sin(double a, double *result) 
{
    status_t exit_code = SUCCESS_CODE;

    if (!fpu_cache_search(FPU_OP_SIN, a, 0.0, result))
    {
        *result = sin(a);
        fpu_cache_store(FPU_OP_ADD, a, 0.0, *result);
    }
    return exit_code;
}

status_t fpu_cmp(double a, double b, double *result) 
{
    status_t exit_code = SUCCESS_CODE;

    if (!fpu_cache_search(FPU_OP_CMP, a, b, result))
    {
        if (fabs(a - b) < EPS)
            *result = 0;
        else
            *result = (a < b) ? -1 : 1;
        fpu_cache_store(FPU_OP_CMP, a, b, *result);
    }
    return exit_code;
}