#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <cstdio>

using namespace std;

// typedef function<void*(void*)> fun_t;
typedef void *(fun_t)(void *);

class Thread
{
public:
    Thread(int num)
    {
        char namebuffer[64];
        snprintf(namebuffer, sizeof namebuffer, "Thread-%d", num);
        name_ = namebuffer;
    }

    void start(fun_t callback, void *args)
    {
        pthread_create(&tid_, nullptr, callback, args);
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
    pthread_t tid_;
};