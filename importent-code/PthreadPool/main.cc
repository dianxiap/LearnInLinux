#include <ctime>
#include <iostream>
#include "task.hpp"
#include "threadpool.hpp"

using namespace std;

int main()
{
    srand((uint32_t)time(nullptr));
    ThreadPool<Task>* tq=new ThreadPool<Task>();

    tq->ThreadPoolInit();

    const char* op="+-*/%";
    while(1)
    {
        sleep(1);
        int x=rand()%100;
        int y=rand()%100;
        int idx=rand()%5;
        Task t(x,y,op[idx]);
        tq->push(t);
    }
    return 0;
}