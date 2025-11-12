#ifndef HEADER_H
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "api/api.h"
#include "utils.h"
void handle_client_request(int),
    send_error_response(int client_socket, const char* status, const char* error_msg);
#endif // HEADER_H
