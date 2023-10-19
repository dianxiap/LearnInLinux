#ifndef _UDP_SERVER_HPP
#define _UDP_SERVER_HPP

#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "log.hpp"

using namespace std;

#define SIZE 1024

class UdpServer
{
public:
    UdpServer(uint16_t port,string ip="0.0.0.0")
    :_port(port)
    ,_ip(ip)
    ,_sock(0)
    {}

    //从这里开始是新的系统调用，来完成网络功能
    // 初始化服务器
    bool initServer()
    {
        //1.创建套接字
        _sock=socket(AF_INET,SOCK_DGRAM,0);
        if(_sock<0)
        {
            logmessage(FATAL,"%d:%s",errno,strerror(errno));
            exit(2);      
        }
        //2.bind:将用户设置的ip和port在内核中和我们当前的进程强关联

        struct sockaddr_in local;
        bzero(&local,sizeof local);
        local.sin_family=AF_INET;

        // 服务器的ip和端口号未来 也是要发送给对方主机的 -> 所以要先发送到网络
        local.sin_port=htons(_port); //主机序列转网络序列（如果是小段就转成大段）   
        
        // （1）将点分十进制字符串风格ip地址转成4字节风格
        // （2）4字节主机序列 -> 网络序列
        //  inet_addr可一次完成上面两件事
        local.sin_addr.s_addr=inet_addr(_ip.c_str());
        
        if(bind(_sock,(struct sockaddr*)&local,sizeof local)<0)
        {
            logmessage(FATAL,"%d:%s",errno,strerror(errno));
            exit(2); 
        }
        logmessage(NORMAL,"init udp server done ... %s",strerror(errno));
        //done
    }

    // 作为一款网络服务器，是永远不退出的
    // 服务器启动 -> 进程 -> 常驻进程 -> 永远在内存中存在，除非挂了
    void start()
    { 
        char buffer[SIZE];
        for( ; ; )
        {
            //输出型参数
            struct sockaddr_in peer;  
            bzero(&peer,sizeof peer);
            
            //输入输出型参数
            //输入：peer 缓冲区的大小
            //输出：实际读到的peer 
            socklen_t len=0;    

            //1.读取数据
            size_t s=recvfrom(_sock,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&peer,&len);
            if(s>0)
            {
                buffer[s]=0; //我们目前数据当作字符串
                //（1）输出发送的数据信息
                //（2）是谁发送的
            }

            //2.分子和处理数据
            //3.写回数据
        }
    }
   
    ~UdpServer(){}
private:
    //一个服务器，一般必须需要ip地址和port（端口号，16位）
    uint16_t _port;
    string _ip;
    int _sock;
};

#endif