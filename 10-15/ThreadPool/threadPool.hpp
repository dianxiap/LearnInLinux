#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "thread.hpp"

using namespace std;

// 本质是：生产者消费模型
template<class T>
class ThreadPool
{
    ThreadPool()
    {}
    //1.run()
    void run(){}
    //2.push任务让线程池来处理
    void pushtask(){}
    ~ThreadPool()
    {}

private:
    vector<Thread*> threads;
    int num_;
    queue<T> task_queue_;   //任务队列
};