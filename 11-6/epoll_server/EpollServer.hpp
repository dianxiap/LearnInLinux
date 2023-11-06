#ifndef __EPOLL_SERVER_HPP__
#define __EPOLL_SERVER_HPP__

#include <iostream>
#include <string>
#include <functional>
#include <cassert>
#include "Log.hpp"
#include "Sock.hpp"
#include "Epoll.hpp"

using namespace std;

namespace ns_epoll
{
    const static int default_port = 8080;
    const static int gnum = 64;

    //只处理读取
    class EpollServer
    {
        using func_t = function<void(string)>;
    public:
        EpollServer(func_t HandlerRequest,const int &port = default_port) 
        : _port(port), _revs_num(gnum),_HandlerRequest(HandlerRequest)
        {
            // 0.申请对应的空间
            _revs = new struct epoll_event[_revs_num];
            // 1.创建listencosk
            _listensock = Sock::Socket();
            Sock::Bind(_listensock, _port);
            Sock::Listen(_listensock);
            // 2.创建epoll模型
            _epfd = Epoll::CreateEpoll();
            logMessage(DEBUG, "init success, listensock: %d, epfd: %d", _listensock, _epfd); // 3 4
            // 3.将listensock，先添加到epoll中，让epoll帮我们管理起来
            if (Epoll::CtlEpoll(_epfd, EPOLL_CTL_ADD, _listensock, EPOLLIN))
            {
                logMessage(DEBUG, "add listensock to epoll success."); // 3 4
            }
            else
            {
                exit(6);
            }
        }
        void Accepter(int listensock)
        {
            string clientip;
            uint16_t clientport;
            int sock=Sock::Accept(listensock,&clientip,&clientport);
            if(sock<0)
            {
                logMessage(WARNING ,"accept error!");
                return ;
            }
            //不能直接读取! 因为并不清楚，底层是否有数据！
            //将新的sock，添加到epoll
            if (!Epoll::CtlEpoll(_epfd, EPOLL_CTL_ADD, sock, EPOLLIN))return ;
            logMessage(DEBUG, "add new sock : %d to epoll success",sock); 
        
        }
        void Recver(int sock)
        {
            //1.读取数据
            char buffer[10240];
            ssize_t n=recv(sock,buffer,sizeof(buffer)-1,0);
            if(n>0)
            {
                //假设这里读到了一个完整的报文
                buffer[n]=0;
                //2.处理数据
                _HandlerRequest(buffer);
            }
            else if(n==0)
            {
                //1.先在epoll中去除对sock的关心
                bool res=Epoll::CtlEpoll(_epfd,EPOLL_CTL_DEL,sock,0);
                assert(res);
                (void)res;
                //2.再close文件
                close(sock);
                logMessage(NORMAL,"client %d quit, me too...",sock);
            }
            else
            {
                //1.先在epoll中去除对sock的关心
                bool res=Epoll::CtlEpoll(_epfd,EPOLL_CTL_DEL,sock,0);
                assert(res);
                (void)res;
                //2.再close文件
                close(sock);
                logMessage(NORMAL,"client recv %d error, close error sock",sock);
            }
        }
        void HandlerEvents(int n)
        {
            assert(n>0);
            for(int i=0;i<n;i++)
            {
                uint32_t revents=_revs[i].events;
                int sock=_revs[i].data.fd;
                //都事件就绪
                if(revents&EPOLLIN)
                {
                    //1.listensock就绪
                    if(sock==_listensock)Accepter(_listensock);
                    //2.一般sock就绪 - read
                    else Recver(sock);
                }
            }
        }
        // 循环一次
        void loopOnce(int timeout)
        {
            int n = Epoll::WaitEpoll(_epfd, _revs, _revs_num, timeout);
            switch (n)
            {
            case 0:
                logMessage(DEBUG, "timeout..."); // 3 , 4
                break;
            case -1:
                logMessage(WARNING, "opoll wait error: %s", strerror(errno));
                break;
            default:
                // 等待成功
                logMessage(DEBUG,"get a event");
                HandlerEvents(n);
                break;
            }
        }
        void start()
        {
            int timeout=-1;
            while(1)
            {
                loopOnce(timeout);
            }
        }
        ~EpollServer()
        {
            if (_listensock >= 0)
                close(_listensock);
            if (_epfd >= 0)
                close(_epfd);
            if (_revs)
                delete[] _revs;
        }

    private:
        int _listensock;
        int _epfd;
        uint16_t _port;
        struct epoll_event *_revs;
        int _revs_num;
        func_t _HandlerRequest;
    };
}

#endif