#include "common.h"
#include "request_listener.h"
#include "request_handler.h"
#include "lib/thread_pool.h"
#include <csignal>

request_listener *rl;

void sig_handler(int sig) {
    if (sig == SIGINT) {
        printf("CTRL+C get\n");

        // TODO 关闭目前connfd，获取fdset中未关闭fd
        char buf[BUFFER_SIZE];
        //shutdown(rl->get_listenfd(), SHUT_RDWR);

        close(rl->get_listenfd());

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