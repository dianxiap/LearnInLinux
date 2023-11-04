#include <iostream>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
using namespace std;
    
// 设置非阻塞
bool SetNonBlock(int fd)
{   
    int fl = fcntl(fd, F_GETFL); // 在底层获取当前fd对应的文件读写标志位
    if (fl < 0)
        return false;
    fcntl(fd, F_SETFL, fl | O_NONBLOCK); // 设置非阻塞
    return true;
}   
    
int main()
{   
    // 标准输入0，默认阻塞式
    SetNonBlock(0); // 只要设置一次，后续就都是非阻塞了
    char buffer[1024];
    while (1)
    {
        sleep(1);
        errno=0;
        // 非阻塞的时候，我们是以出错的形式返回，告知上层数据没有就绪：
        // a.上层如何甄别是真的出错了
        // b.还是仅仅数据没有就绪呢？
        // 数据就绪了的话，我们就正常读取就行
        ssize_t s = read(0, buffer, sizeof(buffer) - 1); // 出错errno变量会被设置，表明出错的原因
        if (s > 0)
        {
            buffer[s-1] = 0;
            cout << "echo# " << buffer << " errno[---]: " << errno << " errstring: " << strerror(errno) << endl;
        }
        else
        {
            // 如果失败的errno是11，代表底层数据没就绪，并没有出错
            // cout << "read \"error\" "  << " errno: " << errno << " errstring: " << strerror(errno) << endl;
            if(errno==EWOULDBLOCK||errno==EAGAIN)
            {
                cout<<" 当前0号fd数据没有就绪，请下一次再来试试吧"<<endl;
                continue;
            }
            else if(errno==EINTR)
            {
                cout<<"当前IO可能被信号中断，请下一次再来试试吧"<<endl;
                continue;
            }
            else 
            {
                //进行差错处理
            }
        }
    }
    return 0;
}   