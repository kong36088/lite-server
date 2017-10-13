#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

//TODO 在cpp中include

#include "locker.h"
#include "thread_pool.h"
#include <errno.h>
#include <exception>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <stdio.h>
#include <csignal>

template<class T>
class thread_pool {
private:
    int thread_number;  //线程池的线程数
    //int max_task_number;  //任务队列中的最大任务数
    pthread_t *all_threads;   //线程数组
    std::queue<T *> task_queue; //任务队列
    mutex_locker queue_mutex_locker;  //互斥锁
    //sem_locker queue_sem_locker;   //信号量
    cond_locker queue_cond_locker; //cond
    bool is_stop; //是否结束线程
    bool is_quit; //线程池是否销毁

  public:
    thread_pool(int thread_num = 20);

    ~thread_pool();

    bool append_task(T *task);

    void start();

    void stop();

    void destroy();

    pthread_t *get_all_threads();

    int get_thread_number(){
        return thread_number;
    }

  private:
    T *get_task();

    void run();

    static void *worker(void *argv);
};

template<class T>
thread_pool<T>::thread_pool(int thread_num)
        : thread_number(thread_num), is_stop(false), all_threads(NULL) {
    if (thread_num <= 0) {
        printf("thread_num cannot less than 1\n");
    }
    all_threads = new pthread_t[thread_num];
    if (all_threads == NULL) {
        printf("threads cannot init because cannot new array");
    }
}

template<class T>
thread_pool<T>::~thread_pool() {
    stop();
    while(!task_queue.empty()){
        T *ta = get_task();
        delete ta;
    }
    //强行终止线程
    for (int i = 0; i < thread_number;i++){
        //pthread_detach(all_threads[i]);
        pthread_cancel(all_threads[i]);
    }
    if(all_threads != NULL){
        delete[] all_threads;
    }
}

template<class T>
void thread_pool<T>::start() {
    for (int i = 0; i < thread_number; i++) {
        if (pthread_create(&all_threads[i], NULL, worker, this) != 0) {
            delete[] all_threads;
            throw std::exception();
        }
        /*
        if (pthread_detach(all_threads[i])) {
            delete[] all_threads;
            throw std::exception();
        }*/
    }
    is_stop = false;
}

template<class T>
void thread_pool<T>::stop() {
    is_stop = true;
    queue_cond_locker.broadcast();
}

template<class T>
bool thread_pool<T>::append_task(T *task) {
    queue_mutex_locker.lock();

    bool is_signal = task_queue.empty();
    task_queue.push(task);

    queue_mutex_locker.unlock();

    if (is_signal) {
        queue_cond_locker.signal();
    }
    return true;
}

template<class T>
void *thread_pool<T>::worker(void *argv) {
    thread_pool<T> *pool = (thread_pool<T> *) argv;
    pool->run();
    return pool;
}

template<class T>
T *thread_pool<T>::get_task() {
    T *task = NULL;
    queue_mutex_locker.lock();
    if (!task_queue.empty()) {
        task = task_queue.front();
        task_queue.pop();
    }
    queue_mutex_locker.unlock();
    return task;
}

template<class T>
void thread_pool<T>::run() {
    while (!is_stop&&!is_quit) {
        T *task = get_task();
        if (task == NULL) {
            queue_cond_locker.wait();
        } else {
            task->execute();
            delete task;
        }
        
    }
}

template<class T>
void thread_pool<T>::destroy(){
    if(is_quit)
        return;
    is_quit = true;
    queue_cond_locker.broadcast();
}

template<class T>
pthread_t *thread_pool<T>::get_all_threads(){
    return all_threads;
}

#endif //LITE_SERVER_THREAD_POOL_H
