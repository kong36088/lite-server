#ifndef _REQEUST_LISTENER_H_
#define _REQEUST_LISTENER_H_



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