#ifndef _FPU_CORE_H
#define FPU_CORE_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "errors.h"
#include "ops.h"

#define BUF_CMD  32
#define BUF_ARG  64
#define BUF_OUT  64

status_t handle_request(int client_socket, int *len, char *out);
void fpu_init(void);

#endif