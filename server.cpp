#include "request_listener.h"
#include "request_handler.h"
#include "thread_pool.h"
#include <csignal>

request_listener *rl;

//TODO 性能提升
void sig_handler(int sig) {
    if (sig == SIGINT) {
        printf("CTRL+C get\n");

        //关闭目前connfd
        char buf[BUFFER_SIZE];
        //shutdown(rl->get_sockfd(), SHUT_RDWR);

        close(rl->get_sockfd());

        printf("server stop\n");
        delete rl;
    }
}

int main(int argc, char *argv[]){
    signal(SIGINT, sig_handler);
    // thread pool
    // listener
    rl = new request_listener(argc, argv);
    
    rl->execute();
    return 0;
}