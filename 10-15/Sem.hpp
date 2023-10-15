#ifndef _SEM_HPP_
#define _SEM_HPP_

#include <iostream>
#include <semaphore.h>

class Sem
{
public:
    Sem(int value)
    {
        sem_init(&sem_,0,value);
    }
    void p()
    {
        sem_wait(&sem_);
    }
    void v()
    {
        sem_post(&sem_);
    }
    ~Sem()
    {
        sem_destroy(&sem_);
    }
private:
    sem_t sem_;
};


#endif 