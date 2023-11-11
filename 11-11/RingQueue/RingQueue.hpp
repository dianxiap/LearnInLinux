#ifndef _Ring_QUEUE_HPP_
#define _Ring_QUEUE_HPP_

#include <iostream>
#include <pthread.h>
#include <vector>
#include "Sem.hpp"

using namespace std;

template<class T>
class RingQueue
{
public:
    RingQueue(int num=5)
        :_rq(num)
        ,_num(num)
        ,c_step(0)
        ,p_step(0)
        ,space_sem(_num)
        ,data_sem(0)
    {
        pthread_mutex_init(&clock,nullptr);
        pthread_mutex_init(&plock,nullptr);
    }
    ~RingQueue()
    {
        pthread_mutex_destroy(&clock);
        pthread_mutex_destroy(&plock);
    }
    void push(const T&in)
    {
        space_sem.p();
        pthread_mutex_lock(&plock);
        _rq[p_step++]=in;
        p_step%=_num;
        pthread_mutex_unlock(&plock);

        data_sem.v();
    }
    void pop(T*out)
    {
        data_sem.p();
        pthread_mutex_lock(&clock);
        *out=_rq[c_step++];
        c_step%=_num;
        pthread_mutex_unlock(&clock);

        space_sem.v();
    }
private:
    vector<T> _rq;
    int _num;
    int c_step;
    int p_step;
    Sem space_sem;
    Sem data_sem;
    pthread_mutex_t clock;
    pthread_mutex_t plock;
};

#endif