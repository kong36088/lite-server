#ifndef _REQEUST_LISTENER_H_
#define _REQEUST_LISTENER_H_

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <exception>
#include "lib/thread_pool.h"
#include "request_handler.h"

class request_listener{
private:
    int argc;
    char **argv;
    int sockfd;
    int connfd;    

  public:
    request_listener(int argc, char *argv[]):argc(argc),argv(argv) {
        if (argc != 2) {
            printf("usage : %s port \n", argv[0]);
            throw std::exception();
        }
    }

    ~request_listener();

    void execute();

    int get_sockfd();

    int get_connfd();
};

void request_listener::execute(){
    struct sockaddr_in servaddr, client;
    int port = atoi(argv[1]); // port
    // server sockaddr_in
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("socket error.ret:%d\n", sockfd);
        throw std::exception();
    }
    int ret = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0) {
        printf("bind error.ret:%d\n", ret);
        throw std::exception();
    }
    ret = listen(sockfd, 10);
    if (ret < 0) {
        printf("listen error.ret:%d\n", ret);
        throw std::exception();
    }
    thread_pool<request_handler> handler_pool(8);
    handler_pool.start();
    
    printf("server start.\n");

    while (1) {
        socklen_t len = sizeof(client);
        // new connection
        connfd = accept(sockfd, (struct sockaddr *)&client, &len);
        if(connfd == -1){
            break;
        }
        printf("accept a new connection.connfd=%d\n", connfd);        
        request_handler *ta = new request_handler(connfd);
        handler_pool.append_task(ta);
    }
    handler_pool.stop();
    handler_pool.destroy();
}

request_listener::~request_listener(){}

int request_listener::get_sockfd(){
    return sockfd;
}

int request_listener::get_connfd(){
    return connfd;
}

#endif