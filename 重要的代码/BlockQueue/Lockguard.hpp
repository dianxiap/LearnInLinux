#pragma once

#include <iostream>
#include <pthread.h>

class Mutex
{
public:
    Mutex(pthread_mutex_t *pmtx)
        :_pmtx(pmtx)
    {}
    //加锁
    void lock()
    {
        pthread_mutex_lock(_pmtx);
    }
    //解锁
    void unlock()
    {
        pthread_mutex_unlock(_pmtx);
    }
    ~Mutex()
    {
    }
private:
    pthread_mutex_t *_pmtx;
};

class LockGuard
{
public:
    LockGuard(pthread_mutex_t *mtx)
        :_mtu(mtx)
    {
        _mtu.lock();
    }
    ~LockGuard()
    {
        _mtu.unlock();
    }

private:
    Mutex _mtu;
};