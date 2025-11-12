#!/bin/bash
gcc src/{main.c,request.c,response.c,api/{api.c,send_health.c,send_folder.c,send_file.c},utils/utils.c} -o web_server
./web_server
rm -f web_server
