#pragma once

#include <iostream>
#include <signal.h>
#include <functional>
#include "Sock.hpp"

class HttpServer
{
public:
    using func_t = std::function<void(int)>;

private:
    int listensock_;
    uint16_t port_;
    Sock sock;
    func_t func_;

public:
    HttpServer(const uint16_t &port, func_t func) : port_(port), func_(func)
    {
        listensock_ = sock.Socket();
        sock.Bind(listensock_, port_);
        sock.Listen(listensock_);
    }
    void Start()
    {
        signal(SIGCHLD, SIG_IGN);
        for (;;)
        {
            std::string clientIp;
            uint16_t clientPort = 0;
            int sockfd = sock.Accept(listensock_, &clientIp, &clientPort);
            if (sockfd < 0)
                continue;
            if (fork() == 0)
            {
                close(listensock_);
                func_(sockfd);
                close(sockfd);
                exit(0);
            }
            close(sockfd);
        }
    }
    ~HttpServer()
    {
        if (listensock_ >= 0)
            close(listensock_);
    }
};