#include "fpu_core.h"

void fpu_init(void) 
{
    for (int i = 0; i < FPU_CACHE_SIZE; i++)
        fpu_cache[i].is_valid = 0;
    fpu_cache_index = 0;
}

status_t recv_line(int client_socket, char *buf, size_t maxlen) 
{
    status_t exit_code = SUCCESS_CODE;
    size_t i = 0;
    char c;
    int count;
    bool is_parsing = true;

    while ((is_parsing) && (i < maxlen - 1)) 
    {
        count = recv(client_socket, &c, 1, 0);
        if (count <= 0)
            return ERR_SOCKET;
        else if (c == '\n')
            is_parsing = false;
        else
            buf[i++] = c;
    }
    if (exit_code == SUCCESS_CODE)
        buf[i] = '\0';

    return exit_code;
}

status_t handle_request(int client_socket, int *len, char *out)
{
    status_t exit_code = SUCCESS_CODE;

    char cmd[BUF_CMD];
    char sarg1[BUF_ARG];
    char sarg2[BUF_ARG];
    double result = 0.0;

    if (recv_line(client_socket, cmd,  BUF_CMD) != SUCCESS_CODE) 
        exit_code = ERR_SOCKET;
    else if (recv_line(client_socket, sarg1, BUF_ARG) != SUCCESS_CODE) 
        exit_code = ERR_SOCKET;
    else if (recv_line(client_socket, sarg2, BUF_ARG) != SUCCESS_CODE)
        exit_code = ERR_SOCKET;
    
    // printf("CMD: '%s'\n", cmd);
    // printf("ARG1: '%s'\n", sarg1);
    // printf("ARG2: '%s'\n", sarg2);

    if (exit_code == SUCCESS_CODE)
    {
        double a = atof(sarg1);
        double b = atof(sarg2);
        
        if (strcmp(cmd, "fadd") == 0)
            exit_code = fpu_add(a, b, &result);
        else if (strcmp(cmd, "fsub") == 0)
            exit_code = fpu_sub(a, b, &result);
        else if (strcmp(cmd, "fdiv") == 0)
            exit_code = fpu_div(a, b, &result);
        else if (strcmp(cmd, "fsin") == 0)
            exit_code = fpu_sin(a, &result);
        else if (strcmp(cmd, "fcmp") == 0)
            exit_code = fpu_cmp(a, b, &result);
        else
            exit_code = ERR_NOT_FOUND;
    }

    // printf("exit_code=%d, result=%.10g\n", exit_code, result);
    if (exit_code == SUCCESS_CODE)
        *len = snprintf(out, BUF_OUT, "%.10g\n", result);

    return exit_code;
}