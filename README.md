# C Web Server

Localhost data transferation - A lightweight HTTP server for local file sharing, folder compression and media streaming.

## Quick Start

### Build and Run
```bash
# Compile
gcc -o web_server src/main.c src/request.c src/response.c src/api/*.c -I./include

# Start server
./web_server
```
or
```bash
# Quick start
./start.sh

```

## API Endpoints

### Health Check
```bash
# Basic status
curl http://localhost:8080/health

# With data
curl -X PUT http://localhost:8080/health -d '{"test":"data"}'
```

### Folder Operations
```bash
# List files
curl http://localhost:8080/folder/properties

# Download compressed folder
curl -s http://localhost:8080/folder/compressed | tar -xzf -
```

### File Transfer
```bash
# Download video file
curl -k -O http://localhost:8080/file
```

## Configuration

Edit paths in `include/api/api.h`:
```c
#define CUSTOM_PATH "path/to/your/file"
#define MP4_PATH "path/to/your/video.mp4"
#define PLUGIN_FOLDER_PATH "../.config/nvim"
#define COMPRESSED_FOLDER_PATH "your_folder_path"
#define PORT 8080
```

## Features

- **File Transfer**: MP4 video streaming with progress monitoring
- **Folder Compression**: Automatic tar.gz creation and download
- **JSON API**: Structured responses with file listings
- **Modular Design**: Easy endpoint addition and customization
- **Memory Safe**: Proper buffer management and error handling

## Development

Add new endpoints:
1. Declare in `include/api/api.h`
2. Implement in `src/api/`
3. Add route to `FIRST_PATH` array

## Deployment

```bash
# Local
./web_server

# External access
ngrok http 8080

# Production
sudo ufw allow 8080
```
