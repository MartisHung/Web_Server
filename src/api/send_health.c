#include "../../include/header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void api_health(int client_socket, const char* method, const char* body) {
    printf("[API] /health %s", method);

    if ((strcmp(method, "GET") && strcmp(method, "PUT") && strcmp(method, "POST") &&
         strcmp(method, "DELETE")) ||
        (!((!strcmp("GET", method)) * (FIRST_PATH[0].method & GET) |
           (!strcmp("PUT", method)) * (FIRST_PATH[0].method & PUT) |
           (!strcmp("POST", method)) * (FIRST_PATH[0].method & POST) |
           (!strcmp("DELETE", method)) * (FIRST_PATH[0].method & DELETE))))
        goto MethodNotAllowed;

    char *timestamp = time(), *health_response;
    
    if (!strcmp("GET", method)) {
        asprintf(&health_response,
                 "{\"status\":\"OK\",\"message\":\"Server is running\",\"timestamp\":\"%s\"}\n",
                 timestamp);
    } else if (!strcmp("PUT", method) && strlen(body) > 0) {
        printf("%s\n", body);
        asprintf(&health_response,
                 "{\"status\":\"OK\",\"message\":\"Server is "
                 "running\",\"timestamp\":\"%s\",\"requesting_data\":\"%s\"}\n",
                 timestamp, body);
    }
    printf("\t200\n");
    send_json_response(client_socket, "200 OK", health_response);
    free(health_response);
    free(timestamp);
    return;
MethodNotAllowed:
    printf("\t405\n");
    send_error_response(client_socket, "405 Method Not Allowed", "the method is not allowed");
    return;
}
