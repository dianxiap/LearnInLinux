#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "log.hpp"

using namespace std;

class UdpServer
{
public:
    // ./UdpServer 8080
    UdpServer(uint16_t port, string ip = "0.0.0.0")
        : _port(port), _ip(ip), _sock(-1)
    {
    }

    void init()
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
        local.sin_addr.s_addr = inet_addr(_ip.c_str());
        if (bind(_sock, (struct sockaddr *)&local, sizeof local) == -1)
        {
            logMessage(FATAL, "bind error, %d:%s", errno, strerror(errno));
            exit(2);
        }
        logMessage(DEBUG, "bind success, %d", _sock);
    }
    // 3.运行服务端
    void start()
    {
        char inbuffer[1024];
        while (1)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof peer;
            ssize_t s = recvfrom(_sock, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr *)&peer, &len);
            if (s > 0)
                inbuffer[s] = 0;
            else if (s == -1)
            {
                logMessage(WARINING, "recvfrom error, %d:%s", _sock, strerror(errno));
                continue;
            }
            //网络转主机
            string peerip=inet_ntoa(peer.sin_addr);
            uint16_t peerport=ntohs(peer.sin_port);
            logMessage(NOTICE,"[%s:%d]#  %s",peerip.c_str(),peerport,inbuffer);
        }
    }
private:
    uint16_t _port;
    string _ip;
    int _sock;
};

int main(int argc,char* argv[])
{
    if(argc!=2)
    {
        cout<<"Usage:\n\t"<<argv[0]<<" port [ip]"<<endl;
        exit(2);
    }
    uint16_t port=atoi(argv[1]);
    UdpServer svr(port);
    svr.init();
    svr.start();
    return 0;
}