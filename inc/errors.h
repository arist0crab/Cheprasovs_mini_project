#ifndef __ERRORS_H__
#define __ERRORS_H__

typedef enum 
{
    SUCCESS_CODE,
    ERR_DB,
    ERR_IO,
    ERR_MEM,
    ERR_ARGS,
    ERR_CACHE,
    ERR_RANGE,
    ERR_NOT_FOUND,
    ERR_CONNECTION
} status_t;

#endif