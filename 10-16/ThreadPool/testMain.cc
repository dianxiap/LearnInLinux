#include "threadPool.hpp"
#include "Task.hpp"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

using namespace std;

int main()
{
    //cout<<"hello thread pool"<<endl;
    
    srand((unsigned long)time(nullptr)^getpid());

    ThreadPool<Task>::getThreadPool()->run();
    // ThreadPool<Task> *tp=new ThreadPool<Task>();
    // tp->run();
    
    // tp->joins();   
    // while(true)
    // 
    // }

    while(1)
    {
        int x=rand()%100+1;
        usleep(rand()%10000+1);
        int y=rand()%10+1;
        Task t(x,y,[](int x,int y)->int{return x+y;});
        logmessage(DEBUG, "制作任务完成: %d+%d=?", x, y);

        // 推送任务到线程池中
        ThreadPool<Task>::getThreadPool()->pushtask(t);

        sleep(1);
    }
    return 0;
}
