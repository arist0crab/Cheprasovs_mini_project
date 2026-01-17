#ifndef __ERRORS_H__
#define __ERRORS_H__

#include <stdio.h>

typedef enum 
{
    SUCCESS_CODE,
    ERR_IO,
    ERR_MEM,
    ERR_RANGE,
    ERR_NOT_FOUND,
    ERR_SOCKET
} status_t;

#endif