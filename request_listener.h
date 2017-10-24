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
#include <sys/select.h>

class request_listener{
private:
    int argc;
    char **argv;
    int listenfd;
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

    int get_listenfd();

    int get_connfd();
};

void request_listener::execute(){
    struct sockaddr_in servaddr, client;
    int port = atoi(argv[1]); // port
    int maxfd, nready;
    fd_set rset, allset;
    // server sockaddr_in
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // create socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        printf("socket error.ret:%d\n", listenfd);
        throw std::exception();
    }
    int ret = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0) {
        printf("bind error.ret:%d\n", ret);
        throw std::exception();
    }
    ret = listen(listenfd, 10);
    if (ret < 0) {
        printf("listen error.ret:%d\n", ret);
        throw std::exception();
    }
    thread_pool<request_handler> handler_pool(8);
    handler_pool.start();
    
    //select init
    maxfd = listenfd;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    printf("server start.\n");

    while (1) {
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        printf("select return, nready=%d\n", nready);

        if(FD_ISSET(listenfd,&rset)){
            socklen_t len = sizeof(client);
            // new connection
            connfd = accept(listenfd, (struct sockaddr *)&client, &len);
            if(connfd == -1){
                continue;
            }
            //TODO close 时需要注销fd
            FD_SET(connfd, &allset);
            if(connfd > maxfd){
                maxfd = connfd;
            }
            printf("accept a new connection.connfd=%d\n", connfd);        
            request_handler *ta = new request_handler(connfd);
            handler_pool.append_task(ta);
        }
    }
    handler_pool.stop();
    handler_pool.destroy();
}

request_listener::~request_listener(){}

int request_listener::get_listenfd(){
    return listenfd;
}

int request_listener::get_connfd(){
    return connfd;
}

#endif