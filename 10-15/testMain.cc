#include "RingQueue.hpp"
#include <time.h>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

void *consumer(void *args)
{
    RingQueue<int> *rq = (RingQueue<int> *)args;
    while(1)
    {
        sleep(1);
        int x;
        // 1.从环形队列中获取数据或任务
        rq->pop(&x);
        // 2.进行一定的处理 -- 不要忽略他的时间消耗问题
        cout<<"消费："<<x<<" ["<<pthread_self()<<"]"<<endl;
    }
}

void *productor(void *args)
{
    RingQueue<int> *rq = (RingQueue<int> *)args;
    while (1)
    {
        // 1.构建数据或者任务对象 -- 一般可以从外部获得 -- 不要忽略他的时间消耗
        int x = rand() % 100+1;
        cout<<"生产："<<x<<" ["<<pthread_self()<<"]"<<endl;

        // 2.推送到环形队列
        rq->push(x);  //完成生产过程

    }  
}

int main()
{
    srand((uint64_t)time(nullptr)^getpid());
    RingQueue<int> *rq = new RingQueue<int>();
    //rq->debug();    
    pthread_t c[3], p[2];
    pthread_create(c, nullptr, consumer, (void *)rq);
    pthread_create(c+1, nullptr, consumer, (void *)rq);
    pthread_create(c+2, nullptr, consumer, (void *)rq);
    
    pthread_create(p, nullptr, productor, (void *)rq);
    pthread_create(p+1, nullptr, consumer, (void *)rq);

    for(int i=0;i<3;i++)pthread_join(c[i],nullptr);
    for(int i=0;i<2;i++)pthread_join(p[2],nullptr);
    return 0;
}