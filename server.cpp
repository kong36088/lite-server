//
// Created by JIANGWEILONGGGG on 2017/10/10.
//
#include "task.h"
#include "thread_pool.h"

int main(int argc, char *argv[]){
    if (argc != 2)
    {
        printf("usage : %s port \n", argv[0]);
        return 1;
    } 
    int sockfd, connfd;
    struct sockaddr_in servaddr, client;
    int port = atoi(argv[1]); //port
    // server sockaddr_in
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("socket error.ret:%d\n", sockfd);
        return 1;
    }
    int ret = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0)
    {
        printf("bind error.ret:%d\n", ret);
        return 1;
    }
    ret = listen(sockfd, 10);
    if (ret < 0)
    {
        printf("listen error.ret:%d\n", ret);
        return 1;
    }
    // thread pool
    thread_pool<task> pool(8);
    pool.start();

    printf("server start.\n");
    while (1)
    {
        socklen_t len = sizeof(client);
        //new connection
        connfd = accept(sockfd, (struct sockaddr *)&client, &len);
        printf("accept a new connection.connfd=%d", connfd);
        task *ta = new task(connfd);
        pool.append_task(ta);
    }
    return 0;
}