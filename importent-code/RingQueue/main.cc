#include <iostream>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include "RingQueue.hpp"

using namespace std;

void* consumer(void* args)
{
    RingQueue<int>* rq=(RingQueue<int>*)args;
    while(1)
    {
        int x;
        rq->pop(&x);
        cout<<"消费: "<<x<<"["<<pthread_self()<<"]"<<endl;
    }

}

void *productor(void* args)
{
    RingQueue<int>* rq=(RingQueue<int>*)args;
    while(1)
    {
        sleep(1);

        int x=rand()%10+1;
        cout<<"生产: "<<x<<"["<<pthread_self()<<"]"<<endl;
        rq->push(x);
    }
}

int main()
{
    srand((uint64_t)time(nullptr)^getpid()^114514);

    RingQueue<int>* rq=new RingQueue<int>();
    pthread_t p[3],c[2];

    pthread_create(c,nullptr,consumer,rq);
    pthread_create(c+1,nullptr,consumer,rq);
    pthread_create(p,nullptr,productor,rq);
    pthread_create(p+1,nullptr,productor,rq);
    pthread_create(p+2,nullptr,productor,rq);

    for(int i = 0; i < 2; i++) pthread_join(c[i], nullptr);
    for(int i = 0; i < 3; i++) pthread_join(p[i], nullptr);
    return 0;
}