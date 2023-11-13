#pragma once

#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "RingQueue.hpp"
#include "LockGuard.hpp"
#include "log.hpp"
#include "Thread.hpp"

using namespace std;

namespace ns_server
{

    const static uint16_t default_port = 8080;

    using func_t = function<string(string)>;

    class UdpServer
    {
    public:
        // ./UdpServer 8080
        UdpServer(uint16_t port = default_port)
            : _port(port)
        {
            cout << "Server Prot: " << _port << endl;
            pthread_mutex_init(&_mutex, nullptr);

            _p = new Thread(1, bind(&UdpServer::Recv, this));
            _c = new Thread(2, bind(&UdpServer::Boardcast, this));
        }
        ~UdpServer()
        {
            pthread_mutex_destroy(&_mutex);
            _p->join();
            _c->join();
            delete _p;
            delete _c;
        }
        void Start()
        {
            // 1.获取套接字
            _sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (_sock < 0)
            {
                logMessage(FATAL, "sock error, %d:%s", errno, strerror(errno));
                exit(1);
            }
            logMessage(DEBUG, "sock create success, %d", _sock);

            // 2.绑定
            struct sockaddr_in local;
            bzero(&local, sizeof local);

            local.sin_family = AF_INET;
            // 主机转网络:字符串转整数
            local.sin_port = htons(_port);
            local.sin_addr.s_addr = INADDR_ANY;
            if (bind(_sock, (struct sockaddr *)&local, sizeof local) == -1)
            {
                logMessage(FATAL, "bind error, %d:%s", errno, strerror(errno));
                exit(2);
            }
            logMessage(DEBUG, "bind success, %d", _sock);

            _c->run();
            _p->run();
        }
        // 3.运行服务端
        // void start()
        // {
        //     char inbuffer[1024];
        //     while (1)
        //     {
        //         struct sockaddr_in peer;
        //         socklen_t len = sizeof peer;
        //         ssize_t s = recvfrom(_sock, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr *)&peer, &len);
        //         if (s > 0)
        //             inbuffer[s] = 0;
        //         else if (s == -1)
        //         {
        //             logMessage(WARINING, "recvfrom error, %d:%s", _sock, strerror(errno));
        //             continue;
        //         }
        //         // 网络转主机
        //         string peerip = inet_ntoa(peer.sin_addr);
        //         uint16_t peerport = ntohs(peer.sin_port);
        //         logMessage(NOTICE, "[%s:%d]#  %s", peerip.c_str(), peerport, inbuffer);
        //     }
        // }
        void addUser(const string &name, const struct sockaddr_in &peer)
        {
            // 添加新的用户
            LockGuard ls(&_mutex);
            _onlineuser[name] = peer;
        }

        void Recv()
        {
            char buffer[1024];
            while (1)
            {
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                int n = recvfrom(_sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
                if (n > 0)
                    buffer[n] = 0;
                else
                    continue;
                cout << "[Get a message From " << inet_ntoa(peer.sin_addr) << "-" << ntohs(peer.sin_port) << "]:: " << buffer << endl;

                // 构建用户信息
                string name = inet_ntoa(peer.sin_addr);
                name += "-";
                name += to_string(ntohs(peer.sin_port));

                // 添加用户到登陆表中
                addUser(name, peer);
                // 生产任务添加到环形队列
                _rq.push(name + ">>" + buffer);
            }
        }
        // 分发任务给用户
        void Boardcast()
        {
            while (1)
            {
                // 获取信息
                string sendstring;
                _rq.pop(&sendstring);

                vector<struct sockaddr_in> v;

                {
                    LockGuard lg(&_mutex);
                    for (auto &user : _onlineuser)
                    {
                        v.push_back(user.second);
                    }
                }
                // 给所有用户发送信息
                for (auto &e : v)
                {
                    sendto(_sock, sendstring.c_str(), sendstring.size(), 0, (struct sockaddr *)&e, sizeof(e));
                }
            }
        }

    private:
        uint16_t _port;
        int _sock;
        string _ip;
        unordered_map<string, struct sockaddr_in> _onlineuser; // 保存已连接的客户端
        pthread_mutex_t _mutex;                                // 加锁保护_onlineuser
        RingQueue<string> _rq;
        Thread *_c;
        Thread *_p;
    };

}