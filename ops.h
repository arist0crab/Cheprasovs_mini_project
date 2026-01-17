#ifndef _OPS_H_
#define _OPS_H_

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

#define FPU_CACHE_SIZE 5
#define EPS 1e-7

typedef enum 
{
    FPU_OP_ADD,
    FPU_OP_SUB,
    FPU_OP_DIV,
    FPU_OP_SIN,
    FPU_OP_CMP
} fpu_op_t;

typedef struct 
{
    fpu_op_t fpu_op;
    double a;
    double b;
} fpu_key_t;

typedef struct 
{
    fpu_key_t key;
    double result;
    size_t is_valid;
} fpu_entry_t;

extern fpu_entry_t fpu_cache[FPU_CACHE_SIZE];
extern size_t fpu_cache_index;

/// @brief Adds two floating point numbers together
/// @param a first floating point number
/// @param b second floating point number
/// @param result operation result (floating point number)
/// @return Exit code
status_t fpu_add(double a, double b, double *result);

/// @brief Substructs second floating point number from second
/// @param a first floating point number
/// @param b second floating point number
/// @param result operation result (floating point number)
/// @return Exit code
status_t fpu_sub(double a, double b, double *result);

/// @brief Divides first floating point number by second
/// @param a first floating point number
/// @param b second floating point number
/// @param result operation result (floating point number)
/// @return Exit code
status_t fpu_div(double a, double b, double *result);

/// @brief Applies sin func to the floating point number
/// @param a floating point number (arg)
/// @param result func result (floating point number)
/// @return Exit code
status_t fpu_sin(double a, double *result);

/// @brief Checks which one floating point number is bigger 
/// @param a first floating point number
/// @param b second floating point number
/// @param result operation result: -1, if first < second. 0, if first == second. 1, if first > second (integer)
/// @return Exit code
status_t fpu_cmp(double a, double b, double *result);

#endif