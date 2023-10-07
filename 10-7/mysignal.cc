#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void catchSig(int signum)
{
    cout<<"进程捕捉到了一个信号，正在处理中： "<<signum<<" pid: "<<getpid()<<endl;
}

int main()
{
    // // signal(2,fun)
    // signal(SIGINT,catchSig); // 特定信号的处理动作，一般只有一个
    // // signal函数，仅仅是修改进程对特定信号的后续处理动作，不是直接调用对应的处理工作
    // // 
    
    // while(1)
    // {
    //     cout<<"我是一个进程，我正在运行。。。pid: "<<getpid()<<endl;
    
    //     sleep(1);
    // }
    pid_t id=fork();
    if(id==0)
    {
        sleep(1);
        int a=100;
        a/=0;
        exit(0);
    }

    int status=0;
    waitpid(id,&status,0);
    cout<<"父进程："<<getpid()<<" 子进程："<<id<<
    " exit sig: "<<(status & 0x7f)<<" is core: "<<((status>>7)&1)<<endl;

    return 0;
}