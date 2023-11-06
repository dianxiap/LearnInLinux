#ifndef __POLL_SVR_H__
#define __POLL_SVR_H__

#include <iostream>
#include <string>
#include <vector>
#include <poll.h>
#include <sys/time.h>
#include "Log.hpp"
#include "Sock.hpp"

#define FD_NONE -1

using namespace std;
// select 我们只完成读取，写入和异常不做处理 -- epoll(写完整)
class PollServer
{
public:
    static const int nfds = 100;

public:
    PollServer(const uint16_t &port = 8080) : _port(port), _nfds(nfds)
    {
        _listensock = Sock::Socket();
        Sock::Bind(_listensock, _port);
        Sock::Listen(_listensock);
        logMessage(DEBUG, "%s", "create base socket success");

        _fds = new struct pollfd[_nfds];
        for (int i = 0; i < _nfds; i++)
        {
            _fds[i].fd = FD_NONE;
            _fds[i].events = _fds[i].revents = 0;
        }
        _fds[0].fd = _listensock;
        _fds[0].events = POLLIN ;

        _timeout = 1000;
    }

    void Start()
    {

        while (true)
        {
            int n = poll(_fds, _nfds, _timeout);
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
                HandlerEvent();
                break;
            }
        }
    }

    ~PollServer()
    {
        if (_listensock >= 0)
            close(_listensock);
        if (_fds)
            delete[] _fds;
    }

private:
    void HandlerEvent() // fd_set 是一个集合，里面可能会存在多个sock
    {
        for (int i = 0; i < _nfds; i++)
        {
            // 1. 去掉不合法的fd
            if (_fds[i].fd == FD_NONE)
                continue;
            // 2. 合法的就一定就绪了?不一定
            if (_fds[i].revents & POLLIN)
            {
                // 指定的fd，读事件就绪
                // 读事件就绪：连接时间到来，accept
                if (_fds[i].fd == _listensock)
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
        
        int pos = 1;
        for (; pos < _nfds; pos++)
        {
            if (_fds[pos].fd == FD_NONE)
                break;
        }
        if (pos == _nfds)
        {
            // 可以对struct pollfd进行动态扩容
            logMessage(WARNING, "%s:%d", "poll server already full，close: %d", sock);
            close(sock);
        }
        else
        {
            _fds[pos].fd = sock;
            _fds[pos].events=POLLIN;
        }
    }
    void Recver(int pos)
    {
        // 读事件就绪：INPUT事件到来、recv，read
        logMessage(DEBUG, "message in, get IO event: %d", _fds[pos]);
        // 暂时先不做封装, 此时select已经帮我们进行了事件检测，fd上的数据一定是就绪的，即 本次 不会被阻塞
        // 这样读取有bug吗？有的，你怎么保证以读到了一个完整包文呢？
        char buffer[1024];
        int n = recv(_fds[pos].fd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n] = 0;
            logMessage(DEBUG, "client[%d]# %s", _fds[pos].fd, buffer);
        }
        else if (n == 0)
        {
            logMessage(DEBUG, "client[%d] quit, me too...", _fds[pos].fd);
            // 1. 我们也要关闭不需要的fd
            close(_fds[pos].fd);
            // 2. 不要让select帮我关心当前的fd了
            _fds[pos].fd = FD_NONE;
            _fds[pos].events = 0;
        }
        else
        {
            logMessage(WARNING, "%d sock recv error, %d : %s", _fds[pos].fd, errno, strerror(errno));
            // 1. 我们也要关闭不需要的fd
            close(_fds[pos].fd);
            // 2. 不要让select帮我关心当前的fd了
            _fds[pos].fd = FD_NONE;
            _fds[pos].events = 0;
        }
    }

    void DebugPrint()
    {
        cout << "_fd_array[]: ";
        for (int i = 0; i <_nfds; i++)
        {
            if (_fds[i].fd == FD_NONE)
                continue;
            cout << _fds[i].fd  << " ";
        }
        cout << endl;
    }

private:
    uint16_t _port;
    int _listensock;
    struct pollfd *_fds;
    int _nfds;
    int _timeout;
};

#endif