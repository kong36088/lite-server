#ifndef _LITE_LOCKER_H_
#define _LITE_LOCKER_H_

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

class rw_lock {  
private:  
    pthread_mutex_t mxt;  
    pthread_cond_t cond;  
    int rd_cnt;//等待读的数量  
    int wr_cnt;//等待写的数量  
  
public:  
    rw_lock() :rd_cnt(0), wr_cnt(0) 
    {  
        pthread_mutex_init(&mxt,NULL);  
        pthread_cond_init(&cond,NULL);  
    }

    void read_lock();

    void read_unlock();

    void write_lock();

    void write_unlock();
};  

#endif
