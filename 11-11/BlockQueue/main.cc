#include <iostream>
#include <pthread.h>
#include <ctime>
#include <unistd.h>
#include "Task.hpp"
#include "BlockQueue.hpp"

using namespace std;

int myAdd(int x, int y)
{
    return x + y;
}

void *consumer(void *args)
{
    BlockQueue<Task> *bq = (BlockQueue<Task>*)args;
    while(1)
    {
        sleep(1);
        Task t;
        bq->pop(&t);
        cout<<pthread_self()<<" consumer: "<<t._x<<"+"<<t._y<<"="<<t()<<endl;
    }
    return nullptr;
}

void* productor(void* args)
{
    BlockQueue<Task> *bq=(BlockQueue<Task>*)args;
    while(1)
    {
        sleep(1);

        int x=rand()%10+1;
        usleep(1000);
        int y=rand()%5+1;
        Task t(x,y,myAdd);
        bq->push(t);
        cout<<pthread_self()<<" productor: "<<t._x<<"+"<<t._y<<"=?"<<endl;
    }
    return nullptr;
}

int main()
{
    srand((uint64_t)time(nullptr) ^ getpid() ^ 114514);

    BlockQueue<Task> *bq = new BlockQueue<Task>();

    pthread_t c[2], p[2];
    pthread_create(c, nullptr, consumer, bq);
    pthread_create(c + 1, nullptr, consumer, bq);
    pthread_create(p, nullptr, productor, bq);
    pthread_create(p + 1, nullptr, productor, bq);

    pthread_join(c[0],nullptr);
    pthread_join(c[1],nullptr);
    pthread_join(p[0],nullptr);
    pthread_join(p[1],nullptr);
    delete bq;
    return 0;
}