#ifndef _LOCKER_H_
#define _LOCKER_H_

#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

//信号量
class sem_locker {
private:
    sem_t m_sem;
public:
    sem_locker();

    ~sem_locker();

    bool wait();

    bool add();
};

//互斥锁
class mutex_locker {
private:
    pthread_mutex_t m_mutex;
public:
    mutex_locker();

    ~mutex_locker();

    bool lock();

    bool unlock();
};

//条件变量locker
class cond_locker
{
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;

public:
    cond_locker();
    ~cond_locker();
    bool wait();
    bool signal();
    bool broadcast();
};


#endif
