#pragma once

#include <queue>
#include <iostream>
#include <unistd.h>
#include <memory>
#include <pthread.h>

using namespace std;

template<class T>
class ThreadPool
{
    bool isEmpty()
    {
        return _tq.size()==0;
    }
    void LockQ()
    {
        pthread_mutex_lock(&_mtx);
    }
    void unLockQ()
    {
        pthread_mutex_unlock(&_mtx);
    }
    void Wait()
    {
        pthread_cond_wait(&_cod,&_mtx);
    }
    void Wakeup()
    {
        pthread_cond_signal(&_cod);
    }
public:
    ThreadPool(int num=5):_thread_num(num)
    {
        pthread_mutex_init(&_mtx,nullptr);
        pthread_cond_init(&_cod,nullptr);
    }
    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mtx);
        pthread_cond_destroy(&_cod);
    }
    static void* Routine(void* args)
    {
        pthread_detach(pthread_self());
        ThreadPool* self=(ThreadPool*)args;

        while(1)
        {
            self->LockQ();
            while(self->isEmpty())
                self->Wait();
            
            T task;
            self->pop(task);
            self->unLockQ();

            task.Run();
        }
    }
    void ThreadPoolInit()
    {
        pthread_t tid;
        for(int i=0;i<_thread_num;i++)
            pthread_create(&tid,nullptr,Routine,this);
    }
    void push(const T& task)
    {
        LockQ();
        _tq.push(task);
        unLockQ();
        Wakeup();
    }
    void pop(T &task)
    {
        task=_tq.front();
        _tq.pop();
    }
private:
    queue<T> _tq;
    int _thread_num;
    pthread_mutex_t _mtx;
    pthread_cond_t _cod;
};