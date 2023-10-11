#include <iostream>
#include <unistd.h>
#include <pthread.h>

using namespace std;

void* threadRoutine(void* args)
{
    int i=0;
    int *data=new int[10];
    while(1)
    {
        cout<<"新线程: "<<(char*)args<<" running ... "<<endl;
        sleep(1);
        data[i]=i;
        if(i++==10)break;
        
        // 线程异常
        int a=100;
        a/=0;
    }
    
    cout<<"new thread quit ... "<<endl;
    //return (void*)10;
    //return (void*)data;
}

int main()
{
    pthread_t tid;
    pthread_create(&tid,nullptr,threadRoutine,(void*)"thread 1");

    int* ret=nullptr;
    pthread_join(tid,(void**)&ret);  // 默认阻塞等待新线程退出
    
    cout<<"main thread wait done ... main quit ...: new thread quit:  "<<endl;
    for(int i=0;i<10;i++)
    {
        cout<<ret[i]<<endl;
    }
    // 线程也需要等待
    // while(1)
    // {
    //     cout<<"main线程: "<<" running ... "<<endl;
    //     sleep(1);
    // }

    return 0;
}