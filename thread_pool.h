//
// Created by JIANGWEILONGGGG on 2017/10/10.
//

#ifndef LITE_SERVER_THREAD_POOL_H
#define LITE_SERVER_THREAD_POOL_H

#include "locker.h"
#include <queue>
#include <stdio.h>
#include <exception>
#include <errno.h>
#include <pthread.h>
#include <iostream>

template <class T>
class thread_pool{
private:
    int thread_number;  //线程池的线程数
    //int max_task_number;  //任务队列中的最大任务数
    pthread_t *all_threads;   //线程数组
    std::queue<T *> task_queue; //任务队列
    mutex_locker queue_mutex_locker;  //互斥锁
    //sem_locker queue_sem_locker;   //信号量
    cond_locker queue_cond_locker; //cond
    bool is_stop; //是否结束线程
public:
    thread_pool(int thread_num = 20);
    ~thread_pool();
    bool append_task(T *task);
    void start();
    void stop();

private:
    T *get_task();
    void run();
    static void *worker(void *argv);
};

#endif //LITE_SERVER_THREAD_POOL_H
