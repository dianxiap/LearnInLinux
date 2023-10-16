#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <unistd.h>
#include "thread.hpp"
#include "lockGuard.hpp"
#include "log.hpp"

using namespace std;

const int g_thread_num = 3;

// 本质是：生产者消费模型
template <class T>
class ThreadPool
{
public:
    pthread_mutex_t* getlock()
    {
        return &lock;
    }
    bool isempty()
    {
        return task_queue_.empty();
    }
    void waitcond()
    {
        pthread_cond_wait(&cond,&lock);
    }
    T gettask()
    {
        T t=task_queue_.front();
        task_queue_.pop();
        return t;
    }
private:
    ThreadPool(int thread_num = g_thread_num)
        : num_(thread_num)
    {
        pthread_mutex_init(&lock,nullptr); 
        pthread_cond_init(&cond,nullptr); 
        for (int i = 1; i <= num_; i++)
        {
            threads.push_back(new Thread(i, routine, this));
        }
    }
    ThreadPool(const ThreadPool<T> &other)=delete;
    ThreadPool<T> &operator=(const ThreadPool<T>&other)=delete;
public:
    // 考虑一下多线程使用单例的过程
    static ThreadPool<T>* getThreadPool(int num=g_thread_num)
    {
        // 可以有效减少未来必定要进行加锁检测的问题
        // 拦截大量的在已经创建好单例的时候，剩余线程请求单例的而直接访问锁的行为
        if(nullptr==thread_ptr)
        {
            lockGuard lockguard(&mutex);
            // 但是，未来任何一个线程想获取单例，都必须调用getThreadPool接口
            // 但是，一定会存在大量的申请和释放锁的行为，这个是无用且浪费资源的
            if(nullptr==thread_ptr)
            {
                thread_ptr= new ThreadPool<T>(num);
            }
        }
        return thread_ptr;
    }
    // 1.run(),创建线程池成功启动该函数
    void run()
    {
        for (auto &iter : threads)
        {
            iter->start();
            // cout << iter->name() << " 启动成功 " << endl;
            logmessage(NORMAL,"%s %s",iter->name().c_str(),"启动成功");
        }
    }

    // 每个线程调用该回调函数
    // 有问题，只有routine是类的成员函数，有默认的成员函数，可能出现类型不匹配问题
    // void *routine(void* args)
    static void *routine(void *args)
    {
        ThreadData *td = (ThreadData *)args;
        ThreadPool<T> *tp=(ThreadPool<T> *)td->args_;
        while (true)
        {
            T task;
            {
                lockGuard lockguard(tp->getlock());
                while(tp->isempty()) tp->waitcond();

                // 拿任务  
                task=tp->gettask(); // 任务队列是共享的 -> 将任务从共享，拿到自己的私有空间

            }
            task(td->name_);
        }
    }
    // 2.push任务让线程池来处理
    
    void pushtask(const T &task)
    {
        lockGuard lockguard(&lock);
        task_queue_.push(task);
        //生产一个任务，至少要唤醒一个线程
        pthread_cond_signal(&cond);
    }

    // void joins()
    // {
    //     for (auto &iter : threads)
    //     {
    //         iter->join();
    //     }
    // }

    // void show()
    // {
    //     cout<<"可以传入this，让使用静态方法的线程，访问到线程池内的方法"<<endl;
    // }
    ~ThreadPool()
    {
        for (auto &iter : threads)
        {
            iter->join();
            delete iter;
        }
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&cond);
    }

private:
    vector<Thread *> threads;
    int num_;
    queue<T> task_queue_; // 任务队列

    static ThreadPool<T> *thread_ptr;
    static pthread_mutex_t mutex;
    pthread_mutex_t lock;
    pthread_cond_t cond;
};

template<class T>
ThreadPool<T>* ThreadPool<T>::thread_ptr=nullptr;

template<class T>
pthread_mutex_t ThreadPool<T>::mutex=PTHREAD_MUTEX_INITIALIZER;
