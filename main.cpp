//
// Created by JIANGWEILONGGGG on 2017/10/10.
//
#include "locker.h"
#include "thread_pool.h"

int main() {
    sem_locker sl;
    sl.add();
    sl.wait();

    mutex_locker mt;
    mt.lock();
    mt.unlock();

    cond_locker cdl;

}