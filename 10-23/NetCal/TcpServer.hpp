#pragma once

#include "Sock.hpp"
#include <functional>
#include <pthread.h>
#include <vector>

namespace ns_tcpserver
{
    using func_t = std::function<void(int)>;

    class TcpServer;
    class ThreadData
    {
    public:
        ThreadData(int sock,TcpServer* server)
            :sock_(sock)
            ,server_(server)
        {};
        ~ThreadData(){};
    public:
        int sock_;
        TcpServer* server_;
    };

    class TcpServer
    {
    private:
        static void *ThreadRoutine(void *args)
        {
            pthread_detach(pthread_self());
            ThreadData* td=static_cast<ThreadData*>(args);
            td->server_->excute(td->sock_);
            close(td->sock_);
            delete td;
            return nullptr;
        }

    public:
        TcpServer(const uint16_t &port, const std::string &ip = "0.0.0.0")
        {
            listensock_ = sock_.Socket();
            sock_.Bind(listensock_, port, ip);
            sock_.Listen(listensock_);
        }
        // 绑定一个方法
        void BindService(func_t func)
        {
            func_.push_back(func);
        }
        // 执行方法
        void excute(int sock)
        {
            for(auto &f:func_)
            {
                f(sock);
            }
        }
        void start()
        {
            for (;;)
            {
                std::string clientip;
                uint16_t clientport;
                int sock = sock_.Accept(listensock_, &clientip, &clientport);
                if (sock == -1)
                    continue;
                logMessage(NORMAL, "create new link success, sock: %d", sock);
                pthread_t tid;
                ThreadData *td=new ThreadData(sock,this);
                pthread_create(&tid, nullptr, ThreadRoutine, td);
            }
        }
        ~TcpServer()
        {
            if (listensock_ >= 0)
                close(listensock_);
        }

    private:
        int listensock_;
        Sock sock_;
        // func_t func_;
        std::vector<func_t> func_;
    };
}