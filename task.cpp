//
// Created by JIANGWEILONGGGG on 2017/10/10.
//
#include "task.h"

void task::execute() {
    char buf[BUFFER_SIZE] = '\0';
    int size = 0;
    size = read(connfd, buf, BUFFER_SIZE - 1);
    if (size > 0) {
        char method[10] = '\0';
        char filename[100] = '\0';
        int i = 0, j = 0;
        while (buf[j] != ' ' && buf[j] != '\0') {
            method[i++] = buf[j++];
        }
        j++;
        method[i] = '\0';
        for (i = 0; buf[j] != ' ' && buf[j] != '\0';) {
            filename[i++] = buf[j++];
        }
        filename[i] = 0;

        //比较method

        if (strcmp(method, "GET") == 0) {
            response_get(filename);
        } else if (strcmp(method, "POST") == 0) {
            //TODO 后续大小控制
            char argv[100];
            memset(argv, 0, sizeof(argv));

            int k = 0;
            char *ch = NULL;
            j++;

            while((ch = strstr(argvs, "Content-Length")) == NULL) //查找请求头部中的Content-Length行
            {
                k = 0;
                memset(argvs, 0, sizeof(argvs));
                while(buffer[j] != '\r' && buffer[j] != '\0')
                {
                    argvs[k++] = buffer[j++];
                }
                ++j;
            }
            int length;
            char *str = strchr(argvs, ':');  //获取POST请求数据的长度
            ++str;
            sscanf(str, "%d", &length);
            j = strlen(buffer) - length;    //从请求报文的尾部获取请求数据
            k = 0;
            memset(argvs, 0, sizeof(argvs));
            while(buffer[j] != '\r' && buffer[j] != '\0')
                argvs[k++] = buffer[j++];

            argvs[k] = '\0';
            response_post(filename, argvs);  //POST方法
        } else {


        }


    }
}

void task::response(char *message, int status) {
    char buf[512] = '\0';

    sprintf(buf, "HTTP/1.1 %d OK\r\nConnection: Close\r\ncontent-length:%d\r\n\r\n", status, strlen(message));

    sprintf(buf, "%s%s", buf, message);

    write(connfd, buf, strlen(buf));
}

void task::response_file(int size, int status) {
    char buf[128] = '\0';

    sprintf(buf, "HTTP/1.1 %d OK\r\nConnection: Close\r\ncontent-length:%d\r\n\r\n", status, size);

    write(connfd, buf, strlen(buf));
}


