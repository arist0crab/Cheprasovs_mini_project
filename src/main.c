#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "db.h"
#include "errors.h"

#define PORT 8081
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

static void handle_client(int client_sock);

int main(void)
{
    status_t rc = SUCCESS_CODE;
    int server_fd = 0;
    int client_sock = 0;
    struct sockaddr_in address = {0};
    int addrlen = sizeof(address);
    int opt = 1;
    pid_t pid = 0;
    bool server_running = true;

    rc = init_bd();

    if (rc == SUCCESS_CODE)
    {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0)
            rc = ERR_CONNECTION;
    }

    if (rc == SUCCESS_CODE && setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        rc = ERR_CONNECTION;

    if (rc == SUCCESS_CODE)
    {
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);
    }

    if (rc == SUCCESS_CODE && bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        rc = ERR_CONNECTION;

    if (rc == SUCCESS_CODE && listen(server_fd, MAX_CLIENTS) < 0)
        rc = ERR_CONNECTION;

    while (server_running)
    {
        client_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_sock < 0)
            continue;

        pid = fork();
        if (pid == 0)
        {
            close(server_fd);
            handle_client(client_sock);
            server_running = false;
        }
        else
            close(client_sock);
    }

    close(server_fd);

    return rc;
}

static void handle_client(int client_sock)
{
    status_t rc = SUCCESS_CODE;
    char buffer[BUFFER_SIZE] = {0};
    int bytes_received = 0;
    char key[256] = {0};
    double value = 0.0;
    bool run_flag = true;
    ssize_t cache_index = -1;
    char response[256] = {0};
    char *newline = NULL;
    char *cr = NULL;
    char *cmd_end = NULL;
    char *value_start = NULL;

    cache_t cache = { 0 };
    cache.current_size = 0;

    while (rc == SUCCESS_CODE && run_flag)
    {
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0)
        {
            run_flag = false;
            continue;
        }
        
        buffer[bytes_received] = '\0';
        
        newline = strchr(buffer, '\n');
        if (newline)
            *newline = '\0';
        
        cr = strchr(buffer, '\r');
        if (cr)
            *cr = '\0';
        
        if (strncmp(buffer, "get ", 4) == 0)
        {
            cmd_end = buffer + 3;
            while (*cmd_end == ' ')
                cmd_end++;
            
            strncpy(key, cmd_end, sizeof(key) - 1);
            key[sizeof(key) - 1] = '\0';

            find_in_cache(&cache, key, &cache_index);
            
            if (cache_index >= 0)
            {
                value = cache.items[cache_index].value;
                snprintf(response, sizeof(response), "SUCCESS %.15g\n", value);
                send(client_sock, response, strlen(response), 0);
            }
            else
            {
                rc = get_element_from_bd(key, &value);
                
                if (rc == SUCCESS_CODE)
                {
                    insert_to_cache(&cache, key, value);
                    snprintf(response, sizeof(response), "SUCCESS %.15g\n", value);
                    send(client_sock, response, strlen(response), 0);
                }
                else if (rc == ERR_NOT_FOUND)
                    send(client_sock, "ERROR Key not found\n", 20, 0);
                else
                    send(client_sock, "ERROR Database error\n", 21, 0);
            }
        }
        else if (strncmp(buffer, "insert ", 7) == 0)
        {
            cmd_end = buffer + 6;
            while (*cmd_end == ' ')
                cmd_end++;
            
            value_start = strchr(cmd_end, ' ');
            if (value_start)
            {
                *value_start = '\0';
                strncpy(key, cmd_end, sizeof(key) - 1);
                key[sizeof(key) - 1] = '\0';
                
                value_start++;
                while (*value_start == ' ')
                    value_start++;
                
                value = atof(value_start);
                                
                rc = insert_to_cache(&cache, key, value);
                
                if (rc == SUCCESS_CODE)
                    send(client_sock, "SUCCESS\n", 8, 0);
                else
                    send(client_sock, "ERROR Cache insert failed\n", 26, 0);
            }
            else
                send(client_sock, "ERROR Invalid format. Use: insert key value\n", 44, 0);
        }
        else if (strcmp(buffer, "flush") == 0)
        {
            rc = flush_cache(&cache);
            
            if (rc == SUCCESS_CODE)
                send(client_sock, "SUCCESS Cache flushed to database\n", 34, 0);
            else
                send(client_sock, "ERROR Flush failed\n", 19, 0);
        }
        else if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "quit") == 0)
        {
            flush_cache(&cache);
            send(client_sock, "BYE\n", 4, 0);
            run_flag = false;
        }
        else
            send(client_sock, "ERROR Unknown command.", 45, 0);
    }
    
    for (size_t i = 0; i < cache.current_size; i++)
        free(cache.items[i].key);
    
    close(client_sock);
}