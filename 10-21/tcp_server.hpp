#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <signal.h>
#include <unistd.h>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <pthread.h>
#include <ctype.h>
#include "ThreadPool/log.hpp"
#include "ThreadPool/threadPool.hpp"
#include "ThreadPool/Task.hpp"

// namespace TcpSock{} TcpSock::TcpServer
// static void service(int sock, const std::string &clientip, const uint16_t &clientport)
// {
//     //echo server
//     char buffer[1024];
//     while(true)
//     {
//         // read && write 可以直接被使用！
//         ssize_t s = read(sock, buffer, sizeof(buffer)-1);
//         if(s > 0)
//         {
//             buffer[s] = 0; //将发过来的数据当做字符串
//             std::cout << clientip << ":" << clientport << "# " << buffer << std::endl;
//         }
//         else if(s == 0) //对端关闭连接
//         {
//             logMessage(NORMAL, "%s:%d shutdown, me too!", clientip.c_str(), clientport);
//             break;
//         }
//         else{ //
//             logMessage(ERROR, "read socket error, %d:%s", errno, strerror(errno));
//             break;
//         }

//         write(sock, buffer, strlen(buffer));
//     }
//     close(sock);
// }

static void service(int sock, const std::string &clientip,
                    const uint16_t &clientport, const std::string &thread_name)
{
    // echo server
    //  同时在线10人
    //  所以，我们一般服务器进程业务处理，如果是从连上，到断开，要一直保持这个链接, 长连接
    //  后面有其他方案！
    char buffer[1024];
    while (true)
    {
        // read && write 可以直接被使用！
        ssize_t s = read(sock, buffer, sizeof(buffer) - 1);
        if (s > 0)
        {
            buffer[s] = 0; // 将发过来的数据当做字符串
            std::cout << thread_name << "|" << clientip << ":" << clientport << "# " << buffer << std::endl;
        }
        else if (s == 0) // 对端关闭连接
        {
            logMessage(NORMAL, "%s:%d shutdown, me too!", clientip.c_str(), clientport);
            break;
        }
        else
        { //
            logMessage(ERROR, "read socket error, %d:%s", errno, strerror(errno));
            break;
        }

        write(sock, buffer, strlen(buffer));
    }
    close(sock);
}

static void change(int sock, const std::string &clientip,
                   const uint16_t &clientport, const std::string &thread_name)
{
    // echo server
    //  同时在线10人
    //  所以，我们一般服务器进程业务处理，如果是从连上，到断开，要一直保持这个链接, 长连接
    //  后面有其他方案！
    char buffer[1024];
    // read && write 可以直接被使用！
    ssize_t s = read(sock, buffer, sizeof(buffer) - 1);
    if (s > 0)
    {
        buffer[s] = 0; // 将发过来的数据当做字符串
        std::cout << thread_name << "|" << clientip << ":" << clientport << "# " << buffer << std::endl;
        std::string message;
        char *start = buffer;
        while(*start)
        {
            char c;
            if(islower(*start)) c = toupper(*start);
            else c = *start;
            message.push_back(c);
            start++;
        }

        write(sock, message.c_str(), message.size());
    }
    else if (s == 0) // 对端关闭连接
    {
        logMessage(NORMAL, "%s:%d shutdown, me too!", clientip.c_str(), clientport);
    }
    else
    { //
        logMessage(ERROR, "read socket error, %d:%s", errno, strerror(errno));
    }

    close(sock);
}
static void dictOnline(int sock, const std::string &clientip,
                   const uint16_t &clientport, const std::string &thread_name)
{
    // echo server
    //  同时在线10人
    //  所以，我们一般服务器进程业务处理，如果是从连上，到断开，要一直保持这个链接, 长连接
    //  后面有其他方案！
    char buffer[1024];
    static std::unordered_map<std::string, std::string> dict = {
        {"apple", "苹果"},
        {"bite", "比特"},
        {"banana", "香蕉"},
        {"hard", "好难啊"}
    };
    // read && write 可以直接被使用！
    ssize_t s = read(sock, buffer, sizeof(buffer) - 1);
    if (s > 0)
    {
        buffer[s] = 0; // 将发过来的数据当做字符串
        std::cout << thread_name << "|" << clientip << ":" << clientport << "# " << buffer << std::endl;
        std::string message;
        auto iter = dict.find(buffer);
        if(iter == dict.end()) message = "我不知道...";
        else message = iter->second;
        write(sock, message.c_str(), message.size());
    }
    else if (s == 0) // 对端关闭连接
    {
        logMessage(NORMAL, "%s:%d shutdown, me too!", clientip.c_str(), clientport);
    }
    else
    { //
        logMessage(ERROR, "read socket error, %d:%s", errno, strerror(errno));
    }

    close(sock);
}

// class ThreadData
// {
// public:
//     int _sock;
//     std::string _ip;
//     uint16_t _port;
// };

class TcpServer
{
private:
    const static int gbacklog = 20; // 后面再说
    // static void *threadRoutine(void *args)
    // {
    //     pthread_detach(pthread_self());
    //     ThreadData *td = static_cast<ThreadData *>(args);
    //     service(td->_sock, td->_ip, td->_port);
    //     delete td;

    //     return nullptr;
    // }
public:
    TcpServer(uint16_t port, std::string ip = "0.0.0.0")
        : _listensock(-1), _port(port),
          _ip(ip), _threadpool_ptr(ThreadPool<Task>::getThreadPool())
    {
    }
    void initServer()
    {
        // 1. 创建socket -- 进程和文件
        _listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensock < 0)
        {
            logMessage(FATAL, "create socket error, %d:%s", errno, strerror(errno));
            exit(2);
        }
        logMessage(NORMAL, "create socket success, listensock: %d", _listensock); // 3

        // 2. bind -- 文件 + 网络
        struct sockaddr_in local;
        memset(&local, 0, sizeof local);
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        inet_pton(AF_INET, _ip.c_str(), &local.sin_addr);
        // inet_aton(_ip.c_str(), &local.sin_addr);
        // local.sin_addr.s_addr = _ip.empty() ? INADDR_ANY : inet_addr(_ip.c_str());
        if (bind(_listensock, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            logMessage(FATAL, "bind error, %d:%s", errno, strerror(errno));
            exit(3);
        }

        // 3. 因为TCP是面向连接的，当我们正式通信的时候，需要先建立连接
        if (listen(_listensock, gbacklog) < 0)
        {
            logMessage(FATAL, "listen error, %d:%s", errno, strerror(errno));
            exit(4);
        }

        logMessage(NORMAL, "init server success");
    }
    void start()
    {
        // signal(SIGCHLD, SIG_IGN); // 对SIGCHLD，主动忽略SIGCHLD信号，子进程退出的时候，会自动释放自己的僵尸状态
        _threadpool_ptr->run();
        while (true)
        {
            // sleep(1);
            // 4. 获取连接
            struct sockaddr_in src;
            socklen_t len = sizeof(src);
            // fd(李四，王五等提供服务的服务员) vs _sock(张三 --- 获取新连接)
            int servicesock = accept(_listensock, (struct sockaddr *)&src, &len);
            if (servicesock < 0)
            {
                logMessage(ERROR, "accept error, %d:%s", errno, strerror(errno));
                continue;
            }
            // 获取连接成功了
            uint16_t client_port = ntohs(src.sin_port);
            std::string client_ip = inet_ntoa(src.sin_addr);
            logMessage(NORMAL, "link success, servicesock: %d | %s : %d |\n",
                       servicesock, client_ip.c_str(), client_port);
            // verison4 --- 线程池版本
            // Task t(servicesock, client_ip, client_port, service);
            // Task t(servicesock, client_ip, client_port, change);
            Task t(servicesock, client_ip, client_port, dictOnline);
            _threadpool_ptr->pushTask(t);

            // version 3 --- 多线程版本
            // ThreadData *td = new ThreadData();
            // td->_sock = servicesock;
            // td->_ip = client_ip;
            // td->_port = client_port;
            // pthread_t tid;
            // //在多线程这里用不用进程关闭特定的文件描述符呢？？1 0
            // pthread_create(&tid, nullptr, threadRoutine, td);
            // close(servicesock);

            // pthread_join();
            // version2.1 -- 多进程版
            // pid_t id = fork();
            // if(id == 0)
            // {
            //     // 子进程
            //     close(_listensock);
            //     if(fork() > 0/*子进程本身*/) exit(0); //子进程本身立即退出
            //     // 孙子进程， 孤儿进程，OS领养， OS在孤儿进程退出的时候，由OS自动回收孤儿进程！
            //     service(servicesock, client_ip, client_port);
            //     exit(0);
            // }
            // // 父进程
            // waitpid(id, nullptr, 0); //不会阻塞！
            // close(servicesock);

            // 开始进行通信服务啦
            // version 1 -- 单进程循环版 -- 只能够进行一次处理一个客户端，处理完了一个，才能处理下一个
            // 很显然，是不能够直接被使用的！ -- 为什么？ 单进程
            // service(servicesock, client_ip, client_port);

            // version 2.0 -- 多进程版 --- 创建子进程
            // 让子进程给新的连接提供服务，子进程能不能打开父进程曾经打开的文件fd呢？1 0
            // pid_t id = fork();
            // assert(id != -1);
            // if(id == 0)
            // {
            //     // 子进程， 子进程会不会继承父进程打开的文件与文件fd呢？1, 0
            //     // 子进程是来进行提供服务的，需不需要知道监听socket呢？
            //     close(_listensock);
            //     service(servicesock, client_ip, client_port);
            //     exit(0); // 僵尸状态
            // }
            // close(servicesock); // 如果父进程关闭servicesock，会不会影响子进程？？下节课
            // 父进程
            // waitpid(); //
        }
    }
    ~TcpServer() {}

private:
    uint16_t _port;
    std::string _ip;
    int _listensock;
    std::unique_ptr<ThreadPool<Task>> _threadpool_ptr;
};
