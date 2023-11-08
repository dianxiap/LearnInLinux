#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <unistd.h>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <fcntl.h>

class Sock
{
private:
    // listen的第二个参数，意义：底层全连接队列的长度 = listen的第二个参数+1
    const static int gbacklog = 10;

public:
    Sock() {}
    static int Socket()
    {
        int listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (listensock < 0)
        {
            exit(2);
        }
        int opt = 1;
        setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        return listensock;
    }
    static void Bind(int sock, uint16_t port, std::string ip = "0.0.0.0")
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof local);
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &local.sin_addr);
        if (bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            exit(3);
        }
    }
    static void Listen(int sock)
    {
        if (listen(sock, gbacklog) < 0)
        {
            exit(4);
        }
    }
    // 一般经验
    // const std::string &: 输入型参数
    // std::string *: 输出型参数
    // std::string &: 输入输出型参数
    static int Accept(int listensock, std::string *ip, uint16_t *port, int *accept_errno)
    {
        struct sockaddr_in src;
        socklen_t len = sizeof(src);
        *accept_errno = 0;
        int servicesock = accept(listensock, (struct sockaddr *)&src, &len);
        if (servicesock < 0)
        {
            *accept_errno = errno;
            return -1;
        }
        if (port)
            *port = ntohs(src.sin_port);
        if (ip)
            *ip = inet_ntoa(src.sin_addr);
        return servicesock;
    }
    static bool Connect(int sock, const std::string &server_ip, const uint16_t &server_port)
    {
        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(server_port);
        server.sin_addr.s_addr = inet_addr(server_ip.c_str());

        if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0)
            return true;
        else
            return false;
    }
    static bool SetNonBlock(int sock)
    {
        int fl = fcntl(sock, F_GETFL);
        if (fl < 0)
            return false;
        fcntl(sock, F_SETFL, fl | O_NONBLOCK);
        return true;
    }
    ~Sock() {}
};
