# C Web Server

功能豐富的模組化 C 語言 HTTP 服務器，支援檔案下載、資料夾壓縮、影片串流等功能。

## 架構

```
web_server/
├── include/
│   ├── header.h          # 主要標頭檔
│   ├── utils.h           # 工具函數
│   └── api/
│       └── api.h         # API 相關定義
├── src/
│   ├── main.c            # 主程式與伺服器初始化
│   ├── request.c         # 請求解析與路由
│   ├── response.c        # HTTP 響應處理
│   └── api/
│       ├── api.c         # API 核心函數
│       ├── health.c      # 健康檢查端點
│       ├── send_file.c   # 檔案下載功能
│       ├── send_folder.c # 資料夾操作
│       └── send_plugin_folder.c # 插件資料夾
└── build/                # 編譯輸出目錄
```

## 快速開始

### 編譯
```bash
# 使用編譯腳本
./start.sh

# 或手動編譯
gcc -o web_server src/main.c src/request.c src/response.c src/api/*.c -I./include
```

### 運行
```bash
./web_server
```

伺服器啟動後會顯示：
```
╔════════════════════════════════════════════╗
║                                            ║
║      Web Server (local host server)        ║
║                                            ║
╚════════════════════════════════════════════╝

[Server] the server is executing in port 8080
[Message] you can use "ngrok" to NAT Traversal
```

## API 端點

### 健康檢查
| 方法 | 路徑 | 描述 |
|------|------|------|
| GET | `/health` | 基本健康狀態 |
| PUT | `/health` | 帶 body 的健康檢查 |

```bash
# GET 健康檢查
curl http://localhost:8080/health

# PUT 帶資料
curl -X PUT http://localhost:8080/health \
  -H "Content-Type: application/json" \
  -d '{"test":"data"}'
```

### 資料夾操作
| 方法 | 路徑 | 描述 |
|------|------|------|
| GET | `/folder/properties` | 取得資料夾檔案列表 |
| GET | `/folder/compressed` | 下載壓縮資料夾 |

```bash
# 查看資料夾內容
curl http://localhost:8080/folder/properties

# 下載並解壓縮資料夾
curl -s http://localhost:8080/folder/compressed | tar -xzf -

# 下載壓縮檔
curl -k -O http://localhost:8080/folder/compressed
```

### 檔案下載
| 方法 | 路徑 | 描述 |
|------|------|------|
| GET | `/file` | 下載影片檔案 |

```bash
# 下載影片檔案
curl -k -O http://localhost:8080/file
```

## 配置

### 路徑設定
編輯 `include/api/api.h` 中的路徑定義：

```c
#define MP4_PATH "path/to/your/video.mp4"           // 影片檔案路徑
#define CUSTOM_PATH "path/to/your/file"             // 自定義檔案路徑  
#define PLUGIN_FOLDER_PATH "../.config/nvim"        // 插件資料夾路徑
#define COMPRESSED_FOLDER_PATH "your_folder_path"   // 壓縮資料夾路徑
```

### 伺服器設定
編輯 `include/header.h` 中的伺服器配置：

```c
#define PORT 8080              // 伺服器端口
#define BUFFER_SIZE 8192       // 緩衝區大小
#define LISTEN_BACKLOG 5       // 連接佇列長度
```

## 核心功能

### 自動初始化
程式啟動時會自動初始化檔案索引：
```c
void __attribute__((constructor)) init_fetch(){
    mp4_file = get_file(MP4_PATH);
    custom_file = get_file(CUSTOM_PATH);
    compressed_folder = get_compressed_folder(COMPRESSED_FOLDER_PATH);
}
```

### 模組化路由
使用 `Requestables` 結構進行路由匹配：
```c
const Requestables FIRST_PATH[LENGTH_OF_PATHS] = {
    {GET | PUT, 7, "/health"},
    {NONE, 7, "/folder"}, 
    {NONE, 5, "/file"}
};
```

### HTTP 方法支援
- **GET**: 資料查詢與檔案下載
- **PUT**: 資料上傳與更新
- **POST**: 資料創建（預留）
- **DELETE**: 資料刪除（預留）

## 檔案處理功能

### 壓縮下載
- 自動使用 `tar -czf` 壓縮資料夾
- 支援 gzip 格式 (.tar.gz)
- 串流式傳輸，記憶體效率高

### 影片串流
- 支援 MP4 格式
- 設定正確的 MIME type
- 支援大檔案傳輸
- 進度監控

### 檔案列表
- JSON 格式回應
- 包含檔案名稱、大小、類型
- 遞歸目錄掃描

## 開發指南

### 添加新端點
1. 在 `include/api/api.h` 中宣告函數
2. 在 `src/api/` 中實現處理函數
3. 在 `FIRST_PATH` 陣列中添加路由
4. 在 `route_request()` 中添加 case

### 錯誤處理
所有 API 函數都包含完整的錯誤處理：
```c
if (stat(file_path, &st) != 0) {
    send_error_response(client_socket, "404 Not Found", "File not found");
    return;
}
```

## 日誌系統

```
[SERVER] 伺服器在端口 8080 啟動              # 伺服器狀態
[CONNECTION] 客戶端已連接                    # 連接管理
[REQUEST] GET /health                       # 請求記錄
[ROUTER] GET /health                        # 路由匹配
[API] GET /health - Health check request    # API 處理
[API] 健康檢查完成                          # 處理結果
```

## 故障排除

### 編譯錯誤
```bash
# 檢查依賴
sudo apt-get install build-essential

# 清理重建
rm -f web_server
gcc -o web_server src/main.c src/request.c src/response.c src/api/*.c -I./include
```

### 檔案權限
```bash
# 給執行權限
chmod +x start.sh web_server

# 檢查檔案路徑
ls -la "你的檔案路徑"
```

### 網路問題
```bash
# 檢查端口占用
sudo netstat -tlnp | grep :8080

# 測試連接
telnet localhost 8080
```

## 部署建議

### 本地開發
```bash
# 直接運行
./web_server
```

### 外網存取
```bash
# 使用 ngrok
ngrok http 8080

# 或設定防火牆
sudo ufw allow 8080
```

### 生產環境
- 使用 systemd 服務
- 配置 nginx 反向代理
- 設定 SSL 憑證
- 實施日誌輪替

## 技術特點

- **模組化設計** - 清晰的檔案結構與職責分離
- **記憶體安全** - 適當的緩衝區管理與錯誤處理
- **效能最佳化** - 串流式檔案傳輸與索引快取
- **擴展性佳** - 易於添加新端點與功能
- **跨平台** - 標準 C 語言，Linux 相容
- **調試友善** - 詳細的日誌與狀態輸出