#include "../include/header.h"
#include "../include/utils.h"

FILE_WITH_INDEX mp4_file,costum_file,compressed_folder;
void __attribute__((constructor)) init_fetch(){
    mp4_file = get_file(MP4_PATH);
    costum_file = get_file(CUSTOM_PATH);
    compressed_folder = get_compressed_folder(COMPRESSED_FOLDER_PATH);
}

static int init_server_socket(void) {
    int server_socket;
    struct sockaddr_in server_addr;
    int opt = 1;
    
    // 建立 socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("[ERROR] Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // 設定 socket 選項 (允許地址重用)
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[ERROR] setsockopt failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    // 設定伺服器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // 綁定 socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[ERROR] Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    // 開始監聽
    if (listen(server_socket, LISTEN_BACKLOG) < 0) {
        perror("[ERROR] Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    return server_socket;
}


int main(void) {
    int server_socket, client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    printf("╔════════════════════════════════════════════╗\n");
    printf("║                                            ║\n");
    printf("║      Web Server (local host server)        ║\n");
    printf("║                                            ║\n");
    printf("╚════════════════════════════════════════════╝\n\n"); 
    printf("[Server] the server is executing in port 8080\n");
    printf("[Message] you can use \"ngrok\" to NAT Traversal\n");

    
    server_socket = init_server_socket();
    
    while (1){
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("[ERROR] Accept failed");
            continue;
        }

        printf("\n[CONNECTION] ✓ 客戶端已連接\n");

        handle_client_request(client_socket);

        close(client_socket);
        printf("[CONNECTION] ✓ 客戶端已斷開\n");
        printf("════════════════════════════════════════════\n");
    }
    
    if(mp4_file.fp)fclose(mp4_file.fp);
    if(custom_file.fp)fclose(custom_file.fp);
    close(server_socket);
    return EXIT_SUCCESS;
}
