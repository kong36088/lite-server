//
// Created by JIANGWEILONGGGG on 2017/10/10.
//

#include "thread_pool.h"

template<class T>
thread_pool<T>::thread_pool(int thread_num) :thread_number(thread_num), is_stop(false), all_threads(NULL) {
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
    delete[]all_threads;
    stop();
}

template<class T>
void thread_pool<T>::start() {
    for (int i = 0; i < thread_number; i++) {
        if (pthread_create(&all_threads[i], NULL, worker, this) != 0) {
            delete[]all_threads;
            throw std::exception();
        }
        if (pthread_detach(all_threads[i])) {
            delete[]all_threads;
            throw std::exception();
        }
    }
    is_stop = true;
}

template<class T>
thread_pool<T>::stop() {
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
void *thread_pool::worker(void *argv) {
    thread_pool *pool = (thread_pool *) argv;
    pool->run();
    return pool;
}

template<class T>
T *thread_pool::get_task() {
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
    while (!is_stop) {
        T *task = task();
        if (task == NULL) {
            queue_cond_locker.wait();
        } else {
            task->execute();
            delete task;
        }
    }
}