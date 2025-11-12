#ifndef UTILS_H
#define UTILS_H

#define PORT 8080
#define BUFFER_SIZE 8192
#define LISTEN_BACKLOG 5
#define TIME_FORMAT_SIZE 21

#define MP4_PATH "path/to/your/mp4"
#define CUSTOM_PATH "path/to/your/file"
#define COMPRESSED_FOLDER_PATH "path/to/your/folder"
#include <dirent.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
typedef struct {
    FILE* fp;
    long file_size;
} FILE_WITH_INDEX;

typedef struct {
    const uint8_t method;
    const uint8_t length;
    const char path[0xff];
} Requestables;

char* time(),getch();
int kbhit();

FILE_WITH_INDEX get_file(const char* PATH),
    get_compressed_folder(const char * PATH);
extern FILE_WITH_INDEX mp4_file, custom_file,compressed_folder;
#endif
