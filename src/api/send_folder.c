#include "../../include/header.h"
#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
//      /"/folder/properties"   "/folder/compressed
const Requestables FOLDER_SUBPATH[LENGTH_OF_FOLDER_SUBPATH] = {{GET, 11, "/properties"},
                                                               {GET, 11, "/compressed"}};
void api_folder(int client_socket, const char* method, const char* body, const char* endpoint) {

    const char* path = endpoint + FIRST_PATH[1].length;
    int8_t count = 0;
    for (; strncmp(FOLDER_SUBPATH[count].path, path, FOLDER_SUBPATH[count].length) &&
           count <= LENGTH_OF_FOLDER_SUBPATH;
         count++)
        ;
    // // For Debugging
    // printf("%sGET:\t%s\nPUT:\t%s\nPOST:\t%s\nDELETE:\t%s\n%s",
    //        "------------------------------------------------------------------------\n",
    //        (!strcmp("GET", method)) * (FOLDER_SUBPATH[0].method & GET) ? "true" : "false",
    //        (!strcmp("PUT", method)) * (FOLDER_SUBPATH[0].method & PUT) ? "true" : "false",
    //        (!strcmp("POST", method)) * (FOLDER_SUBPATH[0].method & POST) ? "true" : "false",
    //        (!strcmp("DELETE", method)) * (FOLDER_SUBPATH[0].method & DELETE) ? "true" : "false",
    //        "------------------------------------------------------------------------\n");
    if ((strcmp(method, "GET") && strcmp(method, "PUT") && strcmp(method, "POST") &&
         strcmp(method, "DELETE")) ||
        (!((!strcmp("GET", method)) * (FOLDER_SUBPATH[count].method & GET) |
           (!strcmp("PUT", method)) * (FOLDER_SUBPATH[count].method & PUT) |
           (!strcmp("POST", method)) * (FOLDER_SUBPATH[count].method & POST) |
           (!strcmp("DELETE", method)) * (FOLDER_SUBPATH[count].method & DELETE))))
        goto MethodNotAllowed;

    switch (count) {
    case 0:
        api_folder_properties(client_socket, method, body);
        break;
    case 1:
        api_folder_compressed(client_socket, method, body);
        break;
    default:
        send_error_response(client_socket, "404 Not Found", "the endpoint not fount");
        return;
    }
    return;

MethodNotAllowed:
    printf("[API] ✗ %s %s %s\n", method, endpoint, "405 Method Not Allowed");
    char *response_body;
    asprintf(&response_body, "the method %s is not allowed in the endpoint %s", endpoint, path);
    send_error_response(client_socket, "405 Method Not Allowed", response_body);
    free(response_body);
    return;
}

void api_folder_compressed(int client_socket, const char* method, const char* body) {
    printf("[API] %s /folder/compressed\n", method);

    char* response_header;
    asprintf(&response_header,
             "HTTP/1.1 200 OK\r\nContent-Type: application/gzip\r\n"
             "Content-Disposition: attachment; filename=\"plugin_folder.tar.gz\"\r\n"
             "Content-Length: %ld\r\n\r\n",
             compressed_folder.file_size);

    send(client_socket, response_header, strlen(response_header), 0);
    free(response_header);
    // tar.gz file sending
    char buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), compressed_folder.fp)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }
    fseek(compressed_folder.fp, 0, SEEK_SET);

    printf("[API] ✓ Compressed file sent with :%ld bytes\n", compressed_folder.file_size);
    return;
compress_error:
    printf("[API] ✗ Failed to open archive file\n");
    send_error_response(client_socket, "500 Internal Server Error", "compress error");
    return;
}

void api_folder_properties(int client_socket, const char* method, const char* body) {
    printf("[API] %s /folder/properties", method);

    // cheak existence
    struct stat st;
    if (stat(COMPRESSED_FOLDER_PATH, &st) != 0 || !S_ISDIR(st.st_mode)) {
        printf("[Server] ✗ Folder not found: %s\n", COMPRESSED_FOLDER_PATH);
        goto ExternalError;
    }

    DIR* dir = opendir(COMPRESSED_FOLDER_PATH);
    if (!dir) {
        printf("[Server] ✗ Cannot open directory: %s\n", COMPRESSED_FOLDER_PATH);
        goto ExternalError;
    }

    char json_response[4096] = "{\"files\":[";
    struct dirent* entry;
    int first_file = 1;

    while ((entry = readdir(dir)) != NULL) {
        if (!(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")))
            continue;

        char* full_path;
        asprintf(&full_path, "%s/%s", COMPRESSED_FOLDER_PATH, entry->d_name);
        struct stat file_stat;
        if (!stat(full_path, &file_stat)) {
            if (!first_file)
                strcat(json_response, ",");

            char* file_info;
            asprintf(&file_info, "{\"name\":\"%s\",\"size\":%ld,\"type\":\"%s\"}", entry->d_name,
                     file_stat.st_size, S_ISDIR(file_stat.st_mode) ? "directory" : "file");

            strcat(json_response, file_info);
            first_file = 0;
            free(file_info);
        }
        free(full_path);
    }

    strcat(json_response, "]}");
    closedir(dir);

    send_json_response(client_socket, "200 OK", json_response);
    printf("\t%d\n", 200);
    return;

ExternalError:
    printf("\t%d\n", 500);
    send_error_response(client_socket, "500 Internal Server Error", "Cannot access the folder");
}
