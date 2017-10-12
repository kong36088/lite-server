#include "request_listener.hpp"
#include "request_handler.hpp"
#include "thread_pool.h"
#include <csignal>

thread_pool<request_listener> *listener_pool;

void sig_handler(int sig) {
    if (sig == SIGINT) {
        printf("CTRL+C get\n");
        delete listener_pool;
    }
}

int main(int argc, char *argv[]){
    signal(SIGINT, sig_handler);
    // thread pool
    // listener
    listener_pool = new thread_pool<request_listener>(1);
    request_listener *rl = new request_listener(argc, argv);
    pthread_t *ths = listener_pool->get_all_threads();

    listener_pool->append_task(rl);
    listener_pool->start();

    pthread_join(ths[0],NULL);

    printf("server stop\n");
    
    delete rl;
    return 0;
}