#ifndef _Ring_QUEUE_HPP_
#define _Ring_QUEUE_HPP_

#include <iostream>
#include <pthread.h>
#include <vector>
#include "Sem.hpp"

using namespace std;

const int g_default_num=5;

template <class T>
class RingQueue
{
public:
    RingQueue(int default_num=g_default_num)
    :rq_(default_num)
    ,num_(default_num)
    ,c_step(0)
    ,p_step(0)
    ,space_sem_(default_num)
    ,data_sem_(0)
    {
        pthread_mutex_init(&clock,nullptr);
        pthread_mutex_init(&plock,nullptr);
    }

    // 生产者：空间资源；生产者们的临界资源是 下标p_step
    void push(const T &in) 
    {
        // 先申请信号量（0）
        space_sem_.p();
        pthread_mutex_lock(&plock);
        // 一定是竞争成功的生产者线程 -- 就一个！所以内部的还是但消费者但生产者模型
        rq_[p_step++]=in;
        p_step%=num_;
        pthread_mutex_unlock(&plock);
        data_sem_.v();
    }
    
    //消费者：消费资源；消费者们的临界资源是 下标c_step
    void pop(T *out) 
    {
        data_sem_.p();
        pthread_mutex_lock(&clock);
        // 一定是竞争成功的消费者线程 -- 就一个！所以内部的还是但消费者但生产者模型
        *out=rq_[c_step++];
        c_step%=num_;
        pthread_mutex_unlock(&clock);
        space_sem_.v();
    }
    ~RingQueue()
    {
        pthread_mutex_destroy(&clock);
        pthread_mutex_destroy(&plock);
    }
    // void debug()
    // {
    //     cerr<<"size: "<<rq_.size()<<" num: "<<num_<<endl;
    // }
private:
    vector<T> rq_;
    int num_;
    int c_step; //消费位置下标
    int p_step; //生产位置下标
    Sem space_sem_;
    Sem data_sem_;
    pthread_mutex_t clock;
    pthread_mutex_t plock;
};

#endif
