#pragma once

#include <iostream>
#include <pthread.h>

class Mutex
{
public:
    Mutex(pthread_mutex_t *pmtx)
    :_pmtx(pmtx)
    {}
    void lock()
    {
        pthread_mutex_lock(_pmtx);
    }
    void unlock()
    {
        pthread_mutex_unlock(_pmtx);
    }
    ~Mutex()
    {}
private:
    pthread_mutex_t *_pmtx;
};

class lockGuard
{
public:
    lockGuard(pthread_mutex_t *pmtx)
    :_mtx(pmtx)
    {
        _mtx.lock();
    }
    ~lockGuard()
    {
        _mtx.unlock();
    }
private:
    Mutex _mtx;
};