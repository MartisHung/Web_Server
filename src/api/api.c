#include "../../include/header.h"
#include <stdio.h>
const Requestables FIRST_PATH[LENGTH_OF_PATHS] = {{GET | PUT,7,"/health"},{NONE,7,"/folder"},{GET,10,"/video.mp4"}};
// const Requestables AHH[LENGTH_OF_PATHS] = {
//     {GET | PUT, "/health"}, {GET, "/folder"},{GET,"/file"}};
//      /"/folder/properties"   "/folder/compressed     /file/video.mp4"
void send_json_response(int client_socket, const char* status, const char* json_body) {
    char *header;
    asprintf(&header,  "HTTP/1.1 %s\r\nContent-Type: application/json\r\n"
             "Content-Length: %zu\r\nConnection: close\r\n\r\n",status, strlen(json_body));

    send(client_socket, header, strlen(header), 0);
    send(client_socket, json_body, strlen(json_body), 0);
    free(header);
}
