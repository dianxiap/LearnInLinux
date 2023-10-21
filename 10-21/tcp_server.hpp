#pragma once

#include <iostream>
#include <string>
#include <cerrno>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <cassert>
#include <sys/socket.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "log.hpp"

using namespace std;

static void service(int sock,const string &clientip,const uint16_t &clientport)
{
    //echo server
    char buffer[1024];
    while(true)
    {   
        // read && write 可以直接被使用
        ssize_t s=read(sock,buffer,sizeof(buffer)-1);
        if(s>0)
        {
            buffer[s]=0;  //将发过来的数据当作字符串
            cout<<clientip<<":"<<clientport<<"# "<<buffer<<endl;
        }
        else if(s==0)   //对端关闭连接
        {
            logmessage(NORMAL,"%s:%d shutdown, me too!",clientip.c_str(),clientport);
            break;
        }
        else    //读取失败
        {
            logmessage(ERROR,"read socket error, %d:%s",errno,strerror(errno));
            break;
        }

        //写回
        write(sock,buffer,strlen(buffer));

    }
}
class TcpServer
{
    const static int gbacklog = 20; // 后面再解释
public:
    TcpServer(uint16_t port, string ip = "")
        : _port(port), _ip(ip), listensock(-1)
    {
    }

    // 初始化服务器
    void initServer()
    {
        // SPCK_STREAM: 面向字节流（tcp）
        // SOCK_DRRAM: 面向数据报（udp）

        // 1.创建socket -- 进程和文件
        listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (listensock < 0)
        {
            logmessage(FATAL, "create socket error, %d:%s", errno, strerror(errno));
            exit(2);
        }
        logmessage(NORMAL, "create socket success, listensock: %d", listensock); // listensock是文件描述符 默认是3

        // 2.bind -- 文件和网络
        struct sockaddr_in local;
        memset(&local, 0, sizeof local);
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);                                             // 主机转网络
        local.sin_addr.s_addr = _ip.empty() ? INADDR_ANY : inet_addr(_ip.c_str()); // 点分十进制主机序列 -> 网络四字节序列
        if (bind(listensock, (struct sockaddr *)&local, sizeof local) < 0)
        {
            logmessage(FATAL, "bind error, %d:%s", errno, strerror(errno));
            exit(3);
        }

        // 3.因为tcp是面向连接的，所以当我们正式通信的时候，需要先建立链接
        if (listen(listensock, gbacklog) < 0)
        {
            logmessage(FATAL, "listen error, %d:%s", errno, strerror(errno));
            exit(4);
        }

        logmessage(NORMAL, "init server success");
    }

    // 启动服务器
    void Start()
    {
        //对SIGCHLD，主动忽略，子进程推出的时候，会自动释放自己的僵尸状态
        signal(SIGCHLD,SIG_IGN); 
        while (true)
        {
            // sleep(1);
            // 4.获取连接
            struct sockaddr_in src; // 客户端的ip和port
            socklen_t len = sizeof(src);

            // fd vs _sock
            //_sock只是帮助accept获取连接
            // 真正进行网络通信服务的是fd
            int servicesock = accept(listensock, (struct sockaddr *)&src, &len);
            if (servicesock < 0)
            {
                logmessage(ERROR, "accept error, %d:%s", errno, strerror(errno));
                continue;
            }
            // 获取连接成功
            uint16_t client_port = ntohs(src.sin_port);
            string client_ip = inet_ntoa(src.sin_addr);
            logmessage(NORMAL, "link success, servicesock: %d | %s : %d |\n", servicesock, client_ip.c_str(), client_port);
            
            // 5.开始进行通信服务
            //version1 -- 单进程循环版 -- 只能一次处理一个客户端，处理完一个，才能处理下一个
            //因为下面这个service是死循环的，只能同时处理一个
            //service(servicesock,client_ip,client_port);
            
            //version2 -- 多进程版 -- 让子进程给新的连接提供服务
            pid_t id=fork();
            assert(id!=-1);
            if(id==0)
            {
                //子进程
                close(listensock);
                service(servicesock,client_ip,client_port);
                exit(0);
            }
            //父进程
            
            close(servicesock);


        }
    }

    ~TcpServer()
    {
    }

private:
    uint16_t _port;
    string _ip;
    int listensock;
};