#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "errors.h"
#include "fpu_core.h"

#define MAX_MESSAGE_LENGTH 256
#define CLIENTS_QUANTITY 1

int main(void)
{
    status_t exit_code = SUCCESS_CODE;
    // char server_message[MAX_MESSAGE_LENGTH] = "FPU server was reached! <3\n";

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9002);

    int check1 = bind(server_socket, (struct sockaddr*) &addr, sizeof(addr));
    printf("BIND EC: %d\n", check1);

    listen(server_socket, CLIENTS_QUANTITY);

    int client_socket = accept(server_socket, NULL, NULL);

    fpu_init();

    char out[BUF_OUT];
    int len = 0;

    while (exit_code == SUCCESS_CODE)
    {
        exit_code = handle_request(client_socket, &len, out);

        if (exit_code == SUCCESS_CODE)
            exit_code = send(client_socket, out, len, 0);
    }

    close(client_socket);
    close(server_socket);   
}