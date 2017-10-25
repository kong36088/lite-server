#ifndef _REQEUST_LISTENER_H_
#define _REQEUST_LISTENER_H_

#include "lib/fdset_container.h"
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
    request_listener(int argc, char *argv[]);

    ~request_listener();

    void execute();

    int get_listenfd();

    int get_connfd();
};

#endif