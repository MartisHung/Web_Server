#include "../../include/header.h"
#include <dirent.h>
#include <sys/stat.h>
char* time() {
    char* timestamp = (char*)malloc(21);
    FILE* fp = popen("TZ=\"Asia/Taipei\" date \"+%Y-%m-%d %a %H:%M\"", "r");
    if (fp) {
        fgets(timestamp, 21, fp);
        pclose(fp);
    }
    return timestamp;
}

FILE_WITH_INDEX get_file(const char* PATH) {
    struct stat st;
    return (stat(PATH, &st)) ? (FILE_WITH_INDEX){.fp = NULL, .file_size = 0}
                             : (FILE_WITH_INDEX){.fp = fopen(PATH, "rb"), .file_size = st.st_size};
}
FILE_WITH_INDEX get_compressed_folder(const char* PATH) {
    struct stat st;
    if (stat(PATH, &st) != 0 || !S_ISDIR(st.st_mode)) {
        return (FILE_WITH_INDEX){.fp = NULL, .file_size = 0};
    }

    char *temp_file = "/tmp/plugin_folder.tar.gz", *tar_command;

    asprintf(&tar_command, "tar -czf %s -C %s .", temp_file, PATH);/* has freed */
    int ret = system(tar_command);
    free(tar_command);
    
    return (ret) ? (FILE_WITH_INDEX){.fp = NULL, .file_size = 0}
           : (stat(temp_file, &st))
               ? (FILE_WITH_INDEX){.fp = NULL, .file_size = 0}
               : (FILE_WITH_INDEX){.fp = fopen(temp_file, "rb"), .file_size = st.st_size};
}
char getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return (char)ch;
}
int kbhit() {
    struct termios oldt, newt;
    int ch, oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}
