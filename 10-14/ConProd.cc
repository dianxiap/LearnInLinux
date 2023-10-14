// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>

// using namespace std;

// #define TNUM 4
// typedef void*(*func_t)(void* args);

// class ThreadData
// {
// public:
//     ThreadData(const string &name,func_t func)
//     :_name(name)
//     ,_func(func)
//     {}
// public:
//     string _name;
//     func_t _func;
//     pthread_mutex_t *_pmtx;
//     pthread_cond_t *_pcond;
// };

// void* Entry(void* args)
// {

// }

// int main()
// {
//     pthread_mutex_t
//     pthread_t tids[TNUM];
//     //func[TNUM]={func1,func2,func3,func4};

//     for(int i=0;i<TNUM;i++)
//     {
//         string name="Thread ";
//         name+=to_string(i+1);
//         ThreadData *td=new ThreadData()
//         pthread_create(tids+i,nullptr,Entry),;
//     }
// }

// 阻塞队列的生产者消费模型
#include "Task.hpp"
#include "BlockQueue.hpp"

#include <pthread.h>
#include <unistd.h>
#include <ctime>

int myadd(int x,int y)
{
    return x+y;
}

void *consumer(void *args)
{
    BlockQueue<Task> *bqueue = (BlockQueue<Task> *)args;

    while (true)
    {
        // sleep(1);
        // int a;
        // 获取任务
        Task t;
        bqueue->pop(&t);
        // 完成任务
        cout<<"consumer: "<<t._x<<"+"<<t._y<<"="<<t()<<endl;
    }

    return nullptr;
}

void *productor(void *args)
{
    BlockQueue<Task> *bqueue = (BlockQueue<Task> *)args;

    //生产
    // int a=1;
    while (true)
    {
        // 获取任务
        
        // int x=rand()%10+1;
        // usleep(rand()%1000);
        // int y=rand()%5+1;
        // Task t(x,y,myadd);
        
        int x,y;
        cout<<"Please Enter x: ";
        cin>>x;
        cout<<"Please Enter y: ";
        cin>>y;
        Task t(x,y,myadd);
        // 生产任务
        bqueue->push(t);
        cout<<"productor: "<<t._x<<"+"<<t._y<<"=?"<<endl;
        sleep(1);
    }

    return nullptr;
}
int main()
{
    srand((uint64_t)time(nullptr)^getpid()^114514);

    BlockQueue<Task> *bqueue = new BlockQueue<Task>();

    pthread_t c, p;
    pthread_create(&c, nullptr, consumer, bqueue);
    pthread_create(&p, nullptr, productor, bqueue);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);

    delete bqueue;
    return 0;
}
