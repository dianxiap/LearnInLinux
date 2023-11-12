#pragma once

#include <iostream>
#include <queue>
#include <pthread.h>
#include "Lockguard.hpp"

using namespace std;

const int gDefault=5;

template<class T>
class BlockQueue
{
public:
    BlockQueue(int capacity=gDefault):_capacity(capacity)
    {
        pthread_mutex_init(&_mtx,nullptr);
        pthread_cond_init(&_empty,nullptr);
        pthread_cond_init(&_full,nullptr);
    }
    void push(const T&in)
    {
        LockGuard lockgurd(&_mtx);
        //挂起等待条件就绪
        while(isfull())
            pthread_cond_wait(&_full,&_mtx);
        //加入阻塞独立
        _bq.push(in);
        //提醒_empty下的线程有数据了
        pthread_cond_signal(&_empty);
    }
    void pop(T *out)
    {
        LockGuard lockgurd(&_mtx);
        while(isempty())
        {
            pthread_cond_wait(&_empty,&_mtx);
        }
        *out=_bq.front();
        _bq.pop();
        pthread_cond_signal(&_full);
    }
    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mtx);
        pthread_cond_destroy(&_empty);
        pthread_cond_destroy(&_full);
    }
private:
    queue<T> _bq;
    int _capacity;
    pthread_mutex_t _mtx;
    pthread_cond_t _empty;
    pthread_cond_t _full;

    bool isfull()
    {
        return _bq.size()==_capacity;
    }
    bool isempty()
    {
        return _bq.size()==0;
    }
};