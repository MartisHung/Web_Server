#include "../include/header.h"
#include <stdio.h>

void send_error_response(int client_socket, const char* status, const char* error_msg) {
    char *json_response, *header;
    asprintf(&json_response, "{\"error\":true,\"status\":\"%s\",\"message\":\"%s\"}\n", status,
             error_msg);
    asprintf(&header,
             "HTTP/1.1 %s\r\nContent-Type: application/json\r\nContent-Length: %zu\r\nConnection: "
             "close\r\n\r\n",
             status, strlen(json_response));
    send(client_socket, header, strlen(header), 0);
    send(client_socket, json_response, strlen(json_response), 0);
    free(header);
    free(json_response);
    return;
}
