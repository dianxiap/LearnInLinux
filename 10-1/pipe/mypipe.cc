#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <assert.h>
#include <string>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;

//为什么不定义全局buffer？因为有写时拷贝！

int main()
{
    // 1.创建管道
    int pipefd[2] = {0}; // pipefd[0]: 读端，pipefd[1]: 写端
    int n = pipe(pipefd);
    // release模式下assert无效
    assert(n != -1);
    // 所以在n在release只被定义，没有被使用，在release模式下n会出下大量警告
    (void)n;

#ifdef DEBUG
    cout << "pipefd[0]: " << pipefd[0] << endl;
    cout << "pipefd[1]: " << pipefd[1] << endl;
#endif

    // 2.创建子进程
    pid_t id = fork();
    assert(id != -1);
    if (id == 0)
    {
        // 子进程
        // 3.构建单向通信的信道，父进程写入，子进程读取
        // 3.1 关闭子进程不需要的fd
        close(pipefd[1]);
        char buffer[1024];
        while (true)
        {
            ssize_t s = read(pipefd[0], buffer, sizeof(buffer) - 1);
            if (s > 0)
            {
                buffer[s] = 0;
                cout <<"child get a message["<<getpid()<< "] Father said# " << buffer << endl;
            }
        }
        exit(0);
    }
    // 父进程
    // 3.构建单向通信的信道
    //3.1 关闭父进程不需要的fd
    close(pipefd[0]);
    string message="我是父进程，我正在给子进程发消息";
    int count=0;
    char send_buffer[1024];
    while(true)
    {
        //3.2 构建一个变化的字符串
        snprintf(send_buffer,sizeof(send_buffer),"%s[%d] : %d",message.c_str(),getpid(),count++);
        //3.3 写入
        write(pipefd[1],send_buffer,strlen(send_buffer));
        sleep(1);
    }

    pid_t ret=waitpid(id,nullptr,0);
    assert(ret<0);
    (void)ret;

    close(pipefd[1]);
    return 0;
}