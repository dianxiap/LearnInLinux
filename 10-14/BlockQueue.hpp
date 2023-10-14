#pragma once

#include <iostream>
#include <queue>
#include <pthread.h>
#include <mutex>
#include "lockGuard.hpp"

using namespace std;

const int gDefayltCap = 5;
// #define INI_MTX(mtx) pthread_mutex_init(&mtx,nullptr)
// #define INI_COND(cond) pthread_cond_init(&mtx,nullptr)

template <class T>
class BlockQueue
{
private:
    bool isQueueEmpty()
    {
        return _bq.size() == 0;
    }
    bool isQueueFull()
    {
        return _bq.size() == _capacity;
    }

public:
    BlockQueue(int capacity = gDefayltCap)
        : _capacity(capacity)
    {
        pthread_mutex_init(&_mtx, nullptr);
        pthread_cond_init(&_Empty, nullptr);
        pthread_cond_init(&_Full, nullptr);
    }

    // s生产者向阻塞队列传入数据
    void push(const T &in)
    {
        //     pthread_mutex_lock(&_mtx);
        //     // 1.先检测当前的临界资源是否能够满足访问条件

        //     // （1）pthread_cond_wait第二个参数是一把锁，当成功调用wait之后，传入的锁，会被自动释放
        //     // （2）从哪里阻塞挂起，就从哪里唤醒（TODO），被唤醒时，pthread_cond_wait会自动加锁
        //     // （3）while循环防止伪唤醒的情况
        //     while (isQueueFull())
        //         pthread_cond_wait(&_Full, &_mtx);
        //     // 2.访问临界资源
        //     _bq.push(in);
        //     // if (_bq.size() >= _capacity / 2)
        //     // {
        //     //     // 有生产者唤醒消费者
        //     //     pthread_cond_signal(&_Empty);
        //     // }
        //     pthread_cond_signal(&_Empty);
        //     pthread_mutex_unlock(&_mtx);
       
//RAII风格的加锁方式

        lockGuard lockguard(&_mtx); //自动调用构造函数 加锁

        while (isQueueFull())
            pthread_cond_wait(&_Full, &_mtx);
        _bq.push(in);
        pthread_cond_signal(&_Empty);

        // 自动调用lockGuard的析构函数 解锁
    }

    // 从阻塞队列拿数据
    void pop(T *out)
    {
        // pthread_mutex_lock(&_mtx);
        // while (isQueueEmpty())
        //     pthread_cond_wait(&_Empty, &_mtx);
        // *out = _bq.front();
        // _bq.pop();

        // // 由消费者唤醒生产者
        // pthread_cond_signal(&_Full);
        // pthread_mutex_unlock(&_mtx);

//RAII风格的加锁方式

        lockGuard lockguard(&_mtx);
        while (isQueueEmpty())
            pthread_cond_wait(&_Empty, &_mtx);
        *out = _bq.front();
        _bq.pop();
        pthread_cond_signal(&_Full);
    }
    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mtx);
        pthread_cond_destroy(&_Empty);
        pthread_cond_destroy(&_Full);
    }

private:
    queue<T> _bq;          // 阻塞队列
    int _capacity;         // 容量上限
    pthread_mutex_t _mtx;  // 通过互斥锁保证队列安全
    pthread_cond_t _Empty; // 用它来表示阻塞队列是否为空的条件--消费者
    pthread_cond_t _Full;  // 用它来表示阻塞队列是否为满的条件--生产者
};
