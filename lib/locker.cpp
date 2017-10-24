//
// Created by JIANGWEILONGGGG on 2017/10/10.
//

#include "locker.h"

//semaphore
sem_locker::sem_locker() {
    if (sem_init(&m_sem, 0, 0) != 0) {
        printf("sem init error\n");
    }
}

sem_locker::~sem_locker() {
    sem_destroy(&m_sem);
}

bool sem_locker::wait() {
    return sem_wait(&m_sem) == 0;
}

bool sem_locker::add() {
    return sem_post(&m_sem) == 0;
}

//mutex
mutex_locker::mutex_locker() {
    if (pthread_mutex_init(&m_mutex, NULL) != 0) {
        printf("mutex init error!\n");
    }
}

mutex_locker::~mutex_locker() {
    pthread_mutex_destroy(&m_mutex);
}

bool mutex_locker::lock() {
    return pthread_mutex_lock(&m_mutex) == 0;
}

bool mutex_locker::unlock() {
    return pthread_mutex_unlock(&m_mutex) == 0;
}


//条件变量locker
// 初始化 m_mutex and m_cond
cond_locker::cond_locker()
{
    if(pthread_mutex_init(&m_mutex, NULL) != 0)
        printf("mutex init error");
    if(pthread_cond_init(&m_cond, NULL) != 0)
    {   //条件变量初始化是被，释放初始化成功的mutex
        pthread_mutex_destroy(&m_mutex);
        printf("cond init error");
    }
}
// destroy mutex and cond
cond_locker::~cond_locker()
{
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}
//等待条件变量
bool cond_locker::wait()
{
    int ans = 0;
    pthread_mutex_lock(&m_mutex);
    ans = pthread_cond_wait(&m_cond, &m_mutex);
    pthread_mutex_unlock(&m_mutex);
    return ans == 0;
}
//唤醒等待条件变量的线程
bool cond_locker::signal()
{
    return pthread_cond_signal(&m_cond) == 0;
}

//唤醒all等待条件变量的线程
bool cond_locker::broadcast()
{
    return pthread_cond_broadcast(&m_cond) == 0;
}

//读写锁
void rw_lock::read_lock() {  
    pthread_mutex_lock(&mxt);  

    ++rd_cnt;  
    while(wr_cnt > 0)  
        pthread_mutex_wait(&cond, &mxt);  
      
    pthread_mutex_unlock(&mxt);  
}  

void rw_lock::read_unlock() {  
    pthread_mutex_lock(&mxt);  
      
    --rd_cnt;  
    if (rd_cnt == 0 )  
        pthread_cond_signal(&cond);  

    pthread_mutex_unlock(&mxt);  
}  

void rw_lock::write_lock() {  
    pthread_mutex_lock(&mxt);  

    ++wr_cnt;  
    while (wr_cnt + rd_cnt >=2)  
        pthread_cond_wait(&cond, &mxt);  

    pthread_mutex_unlock(&mxt);  
}  

void rw_lock::writer_unlock() {  
    pthread_mutex_lock(&mxt);  

    --wr_cnt;  
    if(wr_cnt==0)  
        pthread_cond_signal(&cond);  

    pthread_mutex_unlock(&mxt);  
}  