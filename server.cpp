#include "request_listener.hpp"
#include "request_handler.hpp"
#include "thread_pool.h"
#include <csignal>

request_listener *rl;

//TODO 性能提升
void sig_handler(int sig) {
    if (sig == SIGINT) {
        printf("CTRL+C get\n");

        shutdown(rl->get_sockfd(), SHUT_RDWR);
        close(rl->get_sockfd());

        printf("server stop\n");
        delete rl;
        exit(0);
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