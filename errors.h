#ifndef __ERRORS_H__
#define __ERRORS_H__

typedef enum
{
    SUCCESS_CODE,
    ERR_IO,
    ERR_MEM,
    ERR_RANGE,
    ERR_NOT_FOUND,
    INT_ZERO_DIVISION, // int
    INT_OVERFLOW // для переполнения при сложении или умножении
} status_t;

#endif
