#ifndef _UDP_SERVER_HPP
#define _UDP_SERVER_HPP

#include "log.hpp"
#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define SIZE 1024

class UdpServer
{
public:
    UdpServer(uint16_t port, string ip = "")
        : _port(port), _ip(ip), _sock(-1)
    {
    }

    // 从这里开始是新的系统调用，来完成网络功能

    //  初始化服务器
    bool initServer()
    {
        // 1.创建套接字
        _sock = socket(AF_INET, SOCK_DGRAM, 0); // 返回文件描述符
        if (_sock < 0)
        {
            logmessage(FATAL, "%d:%s", errno, strerror(errno));
            exit(2);
        }

        // 2.bind:（服务端）将用户设置的ip和port在内核中和我们当前的进程强关联
        struct sockaddr_in local;
        bzero(&local, sizeof local);
        local.sin_family = AF_INET;

        // 该服务器的ip和端口号未来要发送给对端主机 -> 所以要先发送到网络
        local.sin_port = htons(_port); // 主机序列转网络序列（如果是小段就转成大段）

        // （1）将点分十进制字符串风格ip地址 <-> 4字节风格
        // （2）4字节主机序列 -> 网络序列
        //  inet_addr可一次完成上面两件事
        // INADDR_ANY:服务器工作过程中，可以从任意ip中获取数据
        local.sin_addr.s_addr = _ip.empty() ? INADDR_ANY : inet_addr(_ip.c_str());

        if (bind(_sock, (struct sockaddr *)&local, sizeof local) < 0)
        {
            logmessage(FATAL, "%d:%s", errno, strerror(errno));
            exit(2);
        }
        logmessage(NORMAL, "init udp server done ... %s", strerror(errno));
        // done
    }

    // 作为一款网络服务器，是永远不退出的
    // 服务器启动 -> 进程 -> 常驻进程 -> 永远在内存中存在，除非挂了
    // echo 服务器：客户端发送消息，服务端原封不动返回
    void start()
    {
        char buffer[SIZE];
        for (;;)
        {
            // start.读取数据

            // 输出型参数peer：存放发送方的ip地址和端口
            struct sockaddr_in peer;
            bzero(&peer, sizeof peer);

            // 输入输出型参数：固定写法，peer的的大小
            // 输入：peer 缓冲区的大小
            // 输出：实际读到的peer
            socklen_t len = sizeof(peer);

            size_t s = recvfrom(_sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
            
            char result[256];
            char key[64];
            string cmd_echo;
            if (s > 0)
            {
                buffer[s] = 0; // 我们目前数据当作字符串
                // （1）输出发送的数据信息
                // （2）是谁发送的
                // （3）客户发的字符串是指令，服务端执行完毕后返还执行结果
                

                //执行命令
                // FILE* fp=popen(buffer,"r");
                // if(nullptr==fp)
                // {
                //     logmessage(ERROR,"popen: %d:%s",errno,strerror(errno));
                //     continue;
                // }
                // //读取命令执行结果
                // while(fgets(result,sizeof result,fp)!=nullptr)
                // {   
                //     cmd_echo+=result;
                // }
                // fclose(fp);

                uint16_t cli_port = ntohs(peer.sin_port); // 从网络中来的
                string cli_ip = inet_ntoa(peer.sin_addr); // 4字节网络序列ip -> 本主机字符串风格ip
                snprintf(key,sizeof key,"%s-%u",cli_ip.c_str(),cli_port);
                
                logmessage(NORMAL,"key: %s",key);

                auto it=_users.find(key);
                if(it==_users.end())
                {
                    logmessage(NORMAL,"add new use: %s",key);
                    _users.insert({key,peer});
                }
                // printf("[%s:%d]# %s\n", cli_ip.c_str(), cli_port, buffer);
            }
            // 分析和处理数据

            // end.写回数据，接收数据后，再把数据返还对端服务器
            for(auto &iter:_users)
            {
                string sendmessage=key;
                sendmessage+="# ";
                sendmessage+=buffer;   
                logmessage(NORMAL,"push message to: %s",iter.first.c_str());
                sendto(_sock, sendmessage.c_str(), sendmessage.size(), 0, (struct sockaddr *)&iter.second, sizeof iter.second);
            }
        }
    }

    ~UdpServer()
    {
        if (_sock >= 0)
            close(_sock);
    }

private:
    // 一个服务器，一般必须需要ip地址和port（端口号，16位）
    uint16_t _port;
    string _ip;
    int _sock;
    unordered_map<string,struct sockaddr_in> _users;
};

#endif