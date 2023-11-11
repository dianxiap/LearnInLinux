#pragma once

#include <iostream>
#include <semaphore.h>

using namespace std;

class Sem
{
public:
    Sem(int val)
    {
        sem_init(&_sem,0,val);
    }
    void p()
    {
        sem_wait(&_sem);
    }
    void v()
    {
        sem_post(&_sem);
    }
    ~Sem()
    {
        sem_destroy(&_sem);
    }
private:
    sem_t _sem;
};
