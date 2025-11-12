#include "../../include/header.h"
#include <stdio.h>
#include <sys/stat.h>
const Requestables FILE_SUBPATH[LENGTH_OF_FILE_SUBPATH] = {{GET, 10, "/video.mp4"}};
void api_handle_video(int client_socket, const char* method, const char* body) {
    printf("[API] %s /video.mp4", method);
    if (!mp4_file.fp)
        goto ExternalError;
    if ((strcmp(method, "GET") && strcmp(method, "PUT") && strcmp(method, "POST") &&
         strcmp(method, "DELETE")) ||
        (!((!strcmp("GET", method)) * (FILE_SUBPATH[0].method & GET) |
           (!strcmp("PUT", method)) * (FILE_SUBPATH[0].method & PUT) |
           (!strcmp("POST", method)) * (FILE_SUBPATH[0].method & POST) |
           (!strcmp("DELETE", method)) * (FILE_SUBPATH[0].method & DELETE))))
        goto MethodNotAllowed;

    printf("\t200\n");
    // 發送 HTTP header
    char* response_header;
    asprintf(&response_header,
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: video/mp4\r\n"
             "Content-Disposition: attachment; filename=\"video.mp4\"\r\n"
             "Content-Length: %ld\r\n"
             "Accept-Ranges: bytes\r\n"
             "\r\n",
             mp4_file.file_size);

    send(client_socket, response_header, strlen(response_header), 0);
    free(response_header);
    // 發送檔案內容
    char buffer[8192];
    size_t bytes_read;
    long total_sent = 0;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), mp4_file.fp)) > 0) {
        ssize_t sent = send(client_socket, buffer, bytes_read, 0);
        if (sent < 0)
            goto ExternalError;

        total_sent += sent;
    }

    printf("[API] ✓ Video download completed (%ld bytes sent)\n", total_sent);
    fseek(mp4_file.fp, 0, SEEK_SET);
    return;

ExternalError:
    send_error_response(client_socket, "500 Internal Server Error", "server_error");
    printf("[Server] error occur %s\n", (total_sent)    ? "while sending response"
                                        : (mp4_file.fp) ? "before sending response\n"
                                                        : "while file opening (nullptr)");
    return;
MethodNotAllowed:
    printf("405 Method not allowed\n");
    char *response_body;
    asprintf(&response_body, "the method %s is not allowed in the endpoint %s",method ,FILE_SUBPATH[0].path) ;
    send_error_response(client_socket, "405 Method Not Allowed", response_body);
    free(response_body);
    return;

}
