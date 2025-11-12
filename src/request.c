#include "../include/header.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
static void route_request(int client_socket, char* method, char* path, char* body);

void handle_client_request(int client_socket) {
    char buffer[BUFFER_SIZE];

    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        printf("[REQUEST] ✗ Failed to receive data\n");
        return;
    }

    buffer[bytes_received] = '\0';

    char method[16], path[256], body[2048] = {0};
    if (sscanf(buffer, "%15s %255s", method, path) != 2) {
        printf("[REQUEST] ✗ Invalid HTTP request format\n");
        send_error_response(client_socket, "400 Bad Request", "Invalid request format");
        return;
    }

    // 提取 body (如果有的話)
    char* body_start = strstr(buffer, "\r\n\r\n");
    if (body_start) {
        body_start += 4;
        if (strlen(body_start) > 0) {
            strncpy(body, body_start, sizeof(body) - 1);
        }
    }

    route_request(client_socket, method, path, body);
}

static void route_request(int client_socket, char* method, char* path, char* body) {
    printf("[ROUTER] %s %s\n", method, path);
    char count = 0;
    for (; strncmp(FIRST_PATH[count].path, path,FIRST_PATH[count].length) && count <= LENGTH_OF_PATHS;count++);
    //START OF cheaking the methods and endpoint
    if (count > LENGTH_OF_PATHS)
        goto EndPointNotFound;
    
    // // debug_message
    // printf("%sGET:\t%s\nPUT:\t%s\nPOST:\t%s\nDELETE:\t%s\n%s",
    //        "------------------------------------------------------------------------\n",
    //        (!strcmp("GET", method)) * (FIRST_PATH[0].length & GET) ? "true" : "false",
    //        (!strcmp("PUT", method)) * (FIRST_PATH[0].length & PUT) ? "true" : "false",
    //        (!strcmp("POST", method)) * (FIRST_PATH[0].length & POST) ? "true" : "false",
    //        (!strcmp("DELETE", method)) * (FIRST_PATH[0].length & DELETE) ? "true" : "false",
    //        "------------------------------------------------------------------------\n");
    // 
    // if ((strcmp(method, "GET") && strcmp(method, "PUT") && strcmp(method, "POST") &&
    //      strcmp(method, "DELETE")) ||
    //     (!((!strcmp("GET", method)) * (FIRST_PATH[count].length & GET) |
    //        (!strcmp("PUT", method)) * (FIRST_PATH[count].length & PUT) |
    //        (!strcmp("POST", method)) * (FIRST_PATH[count].length & POST) |
    //        (!strcmp("DELETE", method)) * (FIRST_PATH[count].length & DELETE))))
    //     goto MethodNotAllowed;
    //END OF cheaking the methods and endpoint
    if(method[strlen(method)-1] =='\\')*(method+strlen(method)-1)='\0';
    switch (count) {
    case 0:
        api_health(client_socket, method, body);
        return;
        break;
    case 1:
        api_folder(client_socket, method, body,path);
        return;
        break;
    case 2:
        api_handle_video(client_socket,method,body);
        return;
        break;
    case 3:
    default:
        printf("requesting: %s\nmethod: %s", path, method);
        goto EndPointNotFound;
        break;
    }
    char * response_body;
EndPointNotFound:
    asprintf(&response_body,  "the End Point %s not found", path);
    send_error_response(client_socket, "404 Not Found", response_body);
    free(response_body);
    return;
}
