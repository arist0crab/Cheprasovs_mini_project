#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "alu_process.h"
#include "errors.h"
#include <stdio.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 2

void handle_client(int client_sock)
{
    char command_str[11];
    char num1_str[32];
    char num2_str[32];
    long long result;
    status_t rc;
    int run_flag = 1;

    while (run_flag)
    {
        int bytes_received = recv(client_sock, command_str, 10, 0);
        if (bytes_received <= 0)
        {
            run_flag = 0;
        }
        else
        {
            command_str[bytes_received] = '\0';

            bytes_received = recv(client_sock, num1_str, 31, 0);
            if (bytes_received <= 0)
            {
                run_flag = 0;
            }
            else
            {
                num1_str[bytes_received] = '\0';

                bytes_received = recv(client_sock, num2_str, 31, 0);
                if (bytes_received <= 0)
                {
                    run_flag = 0;
                }
                else
                {
                    num2_str[bytes_received] = '\0';

                    long long a = atoll(num1_str);
                    long long b = atoll(num2_str);

                    if (strcmp(command_str, "sum") == 0)
                    {
                        rc = int_sum(a, b, &result);
                    }
                    else if (strcmp(command_str, "sub") == 0)
                    {
                        rc = int_sub(a, b, &result);
                    }
                    else if (strcmp(command_str, "mult") == 0)
                    {
                        rc = int_mult(a, b, &result);
                    }
                    else if (strcmp(command_str, "div") == 0)
                    {
                        rc = int_div(a, b, &result);
                    }
                    else if (strcmp(command_str, "cmp") == 0)
                    {
                        rc = int_cmp(a, b, &result);
                    }
                    else if (strcmp(command_str, "neg") == 0)
                    {
                        rc = int_neg(a, &result);
                    }
                    else
                    {
                        rc = ERR_RANGE;
                    }

                    if (rc == SUCCESS_CODE)
                    {
                        char response[32];
                        int len = snprintf(response, 32, "%lld", result);
                        send(client_sock, response, len, 0);
                    }
                    else if (rc == INT_ZERO_DIVISION)
                    {
                        const char* msg = "Division by zero";
                        send(client_sock, msg, strlen(msg), 0);
                    }
                    else if (rc == INT_OVERFLOW)
                    {
                        const char* msg = "Integer overflow";
                        send(client_sock, msg, strlen(msg), 0);
                    }
                    else if (rc == ERR_RANGE)
                    {
                        const char* msg = "Invalid command";
                        send(client_sock, msg, strlen(msg), 0);
                    }
                    else
                    {
                        const char* msg = "Unknown error";
                        send(client_sock, msg, strlen(msg), 0);
                    }
                }
            }
        }
    }

    close(client_sock);
}

int main(void)
{
    int server_fd, client_sock;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    status_t rc = SUCCESS_CODE;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0)
    {
        rc = ERR_IO;
    }

    if (rc == SUCCESS_CODE)
    {
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);
    }

    if (rc == SUCCESS_CODE && bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        close(server_fd);
        rc = ERR_IO;
    }

    if (rc == SUCCESS_CODE && listen(server_fd, MAX_CLIENTS) < 0)
    {
        close(server_fd);
        rc = ERR_IO;
    }

    while (1)
    {
        client_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_sock >= 0)
        {
            handle_client(client_sock);
        }
    }

    close(server_fd);

    return rc;
}
