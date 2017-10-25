#include "request_handler.h"

const char *document_root = "/var/www/html";

request_handler::request_handler(){
    /*map<string, string> config;
    ReadConfig("./config/config.cfg", config);
    if (config.find("document_root") != config.end())
    {
        document_root = config["document_root"].c_str();
        printf("init document_root=%s\n",document_root);
    }else{
        document_root = "/var/www/html";
    }*/
}

request_handler::request_handler(int fd):connfd(fd){
    request_handler();
}

void request_handler::execute() {
    char buf[BUFFER_SIZE];
    int size = 0;
read:
    size = read(connfd, buf, BUFFER_SIZE - 1);
    if (size > 0) {
        char method[10];
        char filename[100];
        int i = 0, j = 0;
        while (buf[j] != ' ' && buf[j] != '\0') {
            method[i++] = buf[j++];
        }
        j++;
        method[i] = '\0';
        for (i = 0; buf[j] != ' ' && buf[j] != '\0';) {
            filename[i++] = buf[j++];
        }
        filename[i] = '\0';

        //比较method
        printf("Get request,method=%s path=%s\n", method, filename);
        printf("document_root=%s", document_root);
        if (strcmp(method, "GET") == 0) {
            response_get(filename);
        } else if (strcmp(method, "POST") == 0) {
            // TODO 后续大小控制
            char argv[100];
            memset(argv, 0, sizeof(argv));

            int k = 0;
            char *ch = NULL;
            j++;

            while ((ch = strstr(argv, "Content-Length")) == NULL) {//查找请求头部中的Content-Length行
                k = 0;
                memset(argv, 0, sizeof(argv));
                while (buf[j] != '\r' && buf[j] != '\0') {
                    argv[k++] = buf[j++];
                }
                ++j;
            }
            int length;
            char *str = strchr(argv, ':'); //获取POST请求数据的长度
            ++str;
            sscanf(str, "%d", &length);
            j = strlen(buf) - length; //从请求报文的尾部获取请求数据
            k = 0;
            memset(argv, 0, sizeof(argv));
            while (buf[j] != '\r' && buf[j] != '\0')
                argv[k++] = buf[j++];

            argv[k] = '\0';
            response_post(filename, argv); // POST方法
        } else {
            char message[512];
            sprintf(message, "<html><title>Lite-server Error</title>");
            sprintf(message, "%s<body>\r\n", message);
            sprintf(message, "%s 501\r\n", message);
            sprintf(message, "%s <p>%s: Httpd does not implement this method",
                    message, method);
            sprintf(message, "%s<hr><h3>The Lite-Server<h3></body>", message);
            response(message, 501);
        }
        //TIME WAIT
        goto read;
    }else if(size < 0){
        goto read;
    }else{
        //TODO 改造成select模型进行time wait
        close(connfd);
    }
}

void request_handler::response(char *message, int status) {
    char buf[512];

    sprintf(buf,"HTTP/1.1 %d OK\r\nConnection: Close\r\nContent-Length:%d\r\n\r\n", status, (int) strlen(message));

    sprintf(buf, "%s%s", buf, message);

    write(connfd, buf, strlen(buf));
}

void request_handler::response_file(int size, int status) {
    char buf[128];

    sprintf(buf,"HTTP/1.1 %d OK\r\nConnection: Close\r\nContent-Length:%d\r\n\r\n",status, size);

    write(connfd, buf, strlen(buf));
}

void request_handler::response_get(char *filename) {
    char file[200]={'\0'};
    strcpy(file, document_root);

    int i = 0;
    bool is_dynamic = false;
    char argv[20];

    while (filename[i] != '?' && filename[i] != '\0') {
        i++;
    }
    if (filename[i] == '?') {
        int j = i;
        i++;
        int k = 0;
        while (filename[i] != '\0') {
            argv[k++] = filename[i++];
        }
        argv[k] = '\0';
        filename[j] = '\0';
        is_dynamic = true;
    }
    if (strcmp(filename, "/") == 0) {
        // TODO config index file
        strcat(filename, "/index.html");
    } else {
        strcat(file, filename);
    }
    struct stat filestat;
    int ret = stat(file, &filestat);

    if (ret < 0 || S_ISDIR(filestat.st_mode)) {
        show404();
        return;
    }
    if (is_dynamic) {
        if (fork() == 0) {
            //子进程
            dup2(connfd, STDOUT_FILENO);
            execl(file, argv);
        }
        wait(NULL);
    } else {
        int filefd = open(file, O_RDONLY);
        response_file(filestat.st_size, 200);
        sendfile(connfd, filefd, 0, filestat.st_size);
    }
}

void request_handler::response_post(char *filename, char *argvs) {
    char file[100];
    strcpy(file, document_root);

    strcat(file, filename);

    struct stat filestat;
    int ret = stat(file, &filestat);
    if (ret < 0 || S_ISDIR(filestat.st_mode)) // file doesn't exits
    {
        show404();
        return;
    }

    char argv[20];
    int a, b;
    ret = sscanf(argvs, "a=%d&b=%d", &a, &b); //判断参数是否正确
    if (ret < 0 || ret != 2) {
        show404();
        return;
    }
    sprintf(argv, "%d&%d", a, b);
    if (fork() == 0)
        /*创建子进程执行对应的子程序，多线程中，创建子进程，
        只有当前线程会被复制，其他线程就“不见了”，这正符合我们的要求，
        而且fork后执行execl，程序被进程被重新加载*/
    {
        dup2(connfd, STDOUT_FILENO);
        //将标准输出重定向到sockfd,将子程序输出内容写到客户端去。
        execl(file, argv); //执行子程序
    }
    wait(NULL);
}

void request_handler::show404() {
    char message[512];
    sprintf(message, "<html><title>Tinyhttpd Error</title>");
    sprintf(message, "%s<body>\r\n", message);
    sprintf(message, "%s 404\r\n", message);
    sprintf(message, "%s <p>GET: Can't find the file", message);
    sprintf(message, "%s<hr><h3>The Lite-server<h3></body>", message);
    response(message, 404);
}