#ifndef __SELECT_SVR_H__
#define __SELECT_SVR_H__

#include <iostream>
#include <string>
#include <vector>
#include <sys/select.h>
#include <sys/time.h>
#include "Log.hpp"
#include "Sock.hpp"

#define BITS 8
#define NUM (sizeof(fd_set) * BITS)
#define FD_NONE -1

using namespace std;
// select 我们只完成读取，写入和异常不做处理 -- epoll(写完整)
class SelectServer
{
public:
    SelectServer(const uint16_t &port = 8080) : _port(port)
    {
        _listensock = Sock::Socket();
        Sock::Bind(_listensock, _port);
        Sock::Listen(_listensock);
        logMessage(DEBUG, "%s", "create base socket success");
        for (int i = 0; i < NUM; i++)
            _fd_array[i] = FD_NONE;
        // 规定 : _fd_array[0] = _listensock;
        _fd_array[0] = _listensock;
    }

    void Start()
    {
        while (true)
        {
            // struct timeval timeout = {0, 0};
            // 如何看待listensock? 获取新连接，我们把它依旧看做成为IO，input事件，如果没有连接到来呢？阻塞
            // int sock = Sock::Accept(listensock, ...); //不能直接调用accept了
            // 将listensock添加到读文件描述符集中
            // FD_SET(_listensock, &rfds);
            // int n = select(_listensock + 1, &rfds, nullptr, nullptr, &timeout);

            // 1. nfds: 随着我们获取的sock越来越多，随着我们添加到select的sock越来越多，注定了nfds每一次都可能要变化,我们需要对它动态计算
            // 2. rfds/writefds/exceptfds：都是输入输出型参数，输入输出不一定以一样的，所以注定了我们每一次都要对rfds进行重新添加
            // 3. timeout: 都是输入输出型参数，每一次都要进行重置，前提是你要的话
            // 1,2 => 注定了我们必须自己将合法的文件描述符需要单独全部保存起来 用来支持：1. 更新最大fd 2.更新位图结构

            DebugPrint();

            fd_set rfds;
            FD_ZERO(&rfds);
            int maxfd = _listensock;
            for (int i = 0; i < NUM; i++)
            {
                if (_fd_array[i] == FD_NONE)
                    continue;
                FD_SET(_fd_array[i], &rfds);
                if (maxfd < _fd_array[i])
                    maxfd = _fd_array[i];
            }
            // rfds未来，一定会有两类sock，listensock，普通sock
            // 我们select中，就绪的fd会越来越多！
            int n = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
            switch (n)
            {
            case 0:
                // printf("hello select ...\n");
                logMessage(DEBUG, "%s", "time out...");
                break;
            case -1:
                logMessage(WARNING, "select error: %d : %s", errno, strerror(errno));
                break;
            default:
                // 成功的
                logMessage(DEBUG, "get a new link event..."); // 为什么会一直打印连接到来呢？连接已经建立完成，就绪了，但是你没有取走，select要一直通知你！
                HandlerEvent(rfds);
                break;
            }
        }
    }

    ~SelectServer()
    {
        if (_listensock >= 0)
            close(_listensock);
    }

private:
    void HandlerEvent(const fd_set &rfds) // fd_set 是一个集合，里面可能会存在多个sock
    {
        for (int i = 0; i < NUM; i++)
        {
            // 1. 去掉不合法的fd
            if (_fd_array[i] == FD_NONE)
                continue;
            // 2. 合法的就一定就绪了?不一定
            if (FD_ISSET(_fd_array[i], &rfds))
            {
                // 指定的fd，读事件就绪
                // 读事件就绪：连接时间到来，accept
                if (_fd_array[i] == _listensock)
                    Accepter();
                else
                    Recver(i);
            }
        }
    }
    void Accepter()
    {
        string clientip;
        uint16_t clientport = 0;
        // listensock上面的读事件就绪了，表示可以读取了
        // 获取新连接了
        int sock = Sock::Accept(_listensock, &clientip, &clientport); // 这里在进行accept会不会阻塞？不会！
        if (sock < 0)
        {
            logMessage(WARNING, "accept error");
            return;
        }
        logMessage(DEBUG, "get a new line success : [%s:%d] : %d", clientip.c_str(), clientport, sock);
        // read / recv? 不能！为什么不能？我们不清楚该sock上面数据什么时候到来， recv、read就有可能先被阻塞，IO = 等+数据拷贝
        // 谁可能最清楚呢？select！
        // 得到新连接的时候，此时我们应该考虑的是，将新的sock托管给select，让select帮我们进行检测sock上是否有新的数据
        // 有了数据select，读事件就绪，select就会通知我，我们在进行读取，此时我们就不会被阻塞了
        // 要将sock添加 给 select， 其实我们只要将fd放入到数组中即可！
        int pos = 1;
        for (; pos < NUM; pos++)
        {
            if (_fd_array[pos] == FD_NONE)
                break;
        }
        if (pos == NUM)
        {
            logMessage(WARNING, "%s:%d", "select server already full，close: %d", sock);
            close(sock);
        }
        else
        {
            _fd_array[pos] = sock;
        }
    }
    void Recver(int pos)
    {
        // 读事件就绪：INPUT事件到来、recv，read
        logMessage(DEBUG, "message in, get IO event: %d", _fd_array[pos]);
        // 暂时先不做封装, 此时select已经帮我们进行了事件检测，fd上的数据一定是就绪的，即 本次 不会被阻塞
        // 这样读取有bug吗？有的，你怎么保证以读到了一个完整包文呢？
        char buffer[1024];
        int n = recv(_fd_array[pos], buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n] = 0;
            logMessage(DEBUG, "client[%d]# %s", _fd_array[pos], buffer);
        }
        else if (n == 0)
        {
            logMessage(DEBUG, "client[%d] quit, me too...", _fd_array[pos]);
            // 1. 我们也要关闭不需要的fd
            close(_fd_array[pos]);
            // 2. 不要让select帮我关心当前的fd了
            _fd_array[pos] = FD_NONE;
        }
        else
        {
            logMessage(WARNING, "%d sock recv error, %d : %s", _fd_array[pos], errno, strerror(errno));
            // 1. 我们也要关闭不需要的fd
            close(_fd_array[pos]);
            // 2. 不要让select帮我关心当前的fd了
            _fd_array[pos] = FD_NONE;
        }
    }

    void DebugPrint()
    {
        cout << "_fd_array[]: ";
        for (int i = 0; i < NUM; i++)
        {
            if (_fd_array[i] == FD_NONE)
                continue;
            cout << _fd_array[i] << " ";
        }
        cout << endl;
    }

private:
    uint16_t _port;
    int _listensock;
    int _fd_array[NUM];
    // int _fd_write[NUM];
    // std::vector<int> arr;
};

#endif