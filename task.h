//
// Created by JIANGWEILONGGGG on 2017/10/10.
//

#ifndef LITE_SERVER_TASK_H
#define LITE_SERVER_TASK_H

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/wait.h>

char *document_root = "/var/www/html";
const int BUFFER_SIZE = 4096;

class task {
private:
    int connfd;
public:
    task(int fd);

    ~task();

    void execute();

    void response(char *message,int status);  //错误响应

    void response_file(int size,int status);  //静态文件响应

    void response_get(char *filename);

    void response_post(char *filename, char *argv);

};

#endif //LITE_SERVER_TASK_H
