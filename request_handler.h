#ifndef _REQUEST_HANDLER_H_
#define _REQUEST_HANDLER_H_

#include "common.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "thirdparty/get_config.h"

class request_handler {
private:
    int connfd;

public:
    request_handler();

    request_handler(int fd);

    ~request_handler() {
        close(connfd);
    };

    void execute();

    void response(char *message, int status); //错误响应

    void response_file(int size, int status); //静态文件响应

    void response_get(char *filename);

    void response_post(char *filename, char *argv);

    void show404();

    int get_connfd(){
        return connfd;
    }
};

#endif
