#ifndef HEALTH_API_H
#define HEALTH_API_H
#include <stdint.h>
#include "../utils.h"

#define NONE 0X0
#define GET 0x8
#define PUT 0x4
#define POST 0x2
#define DELETE 0x1

#define LENGTH_OF_PATHS 3
#define LENGTH_OF_FILE_SUBPATH 1
#define LENGTH_OF_FOLDER_SUBPATH 2

extern const Requestables FIRST_PATH[LENGTH_OF_PATHS];
extern const Requestables FOLDER_SUBPATH[LENGTH_OF_FOLDER_SUBPATH];

// extern const Requestables AHH[LENGTH_OF_PATHS] ;
void send_json_response(int client_socket, const char*, const char*),
    api_health(int client_socket, const char* method, const char* body),
    api_handle_video(int client_socket, const char* method, const char* body);

//  the function of folder
void api_folder_properties(int client_socket, const char* method, const char* body),
    api_folder_compressed(int client_socket, const char* method, const char* body),
    api_folder(int client_socket, const char* method, const char* body, const char* endpoint);

#endif // HEALTH_API_H
