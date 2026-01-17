#ifndef LOG_H__
#define LOG_H__

#include <stdio.h>
#include "data.h"

// завести переменную для счетчика команд, 
// каждый раз при выполнении команды увеличивать счетчик, кроме некоторых команд(jump - принимает на вход номер команды)

void print_sock_err(void);
void print_connect_err(const char *ip, int port);
void print_ip_err(const char *ip);
void print_help(void);
void print_connect_success(int port);
void print_err_all_connect(void);

#endif