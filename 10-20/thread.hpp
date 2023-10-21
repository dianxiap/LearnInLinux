#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <cstdio>

using namespace std;

// typedef function<void*(void*)> fun_t;
typedef void *(*fun_t)(void *);

class ThreadData
{
public:
    void* args_;
    string name_;
};


class Thread
{
public:
    Thread(int num,fun_t callback, void *args)
    :func_(callback)
    {
        char namebuffer[64];
        snprintf(namebuffer, sizeof namebuffer, "Thread-%d", num);
        name_ = namebuffer;
        tdata_.args_=args;
        tdata_.name_=name_;
    }

    void start()
    {
        pthread_create(&tid_, nullptr, func_, &tdata_);
    }

    void join()
    {
        pthread_join(tid_,nullptr);
    }

    string name()
    {
        return name_;
    }
    ~Thread()
    {
    }

private:
    string name_;
    fun_t func_;
    ThreadData tdata_;
    pthread_t tid_;
};