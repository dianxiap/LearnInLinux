#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "log.hpp"

using namespace std;

class TcpServer
{
public:
    TcpServer(uint16_t port, string ip = "0.0.0.0")
        : _port(port), _ip(ip), _listensock(-1)
    {
    }
    void init()
    {
        // 1.获取套接字
        _listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensock < 0)
        {
            logMessage(FATAL, "_istensock error, %d:%s", errno, strerror(errno));
            exit(2);
        }
        logMessage(DEBUG, "listensock create success, %d", _listensock);
        // 2.绑定
        struct sockaddr_in local;
        bzero(&local, sizeof local);
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = inet_addr(_ip.c_str());

        if (bind(_listensock, (struct sockaddr *)&local, sizeof local)  <0)
        {
            logMessage(FATAL, "bind error, %s", strerror(errno));
            exit(3);
        }
        logMessage(DEBUG, "bind success, %d", _listensock);

        // 3.监听
        if (listen(_listensock, 5) < 0)
        {
            logMessage(FATAL, "listen error, %s", strerror(errno));
            exit(4);
        }
        logMessage(DEBUG, "listen success, %d", _listensock);
    }

    void loop()
    {
        // 4.获取连接

        while (1)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof peer;
            int sock = accept(_listensock, (struct sockaddr *)&peer, &len);
            if (sock < 0)
            {
                logMessage(WARINING, "accept error, %s", strerror(errno));
                continue;
            }
            uint16_t peerport = ntohs(peer.sin_port);
            string peerip = inet_ntoa(peer.sin_addr);
            logMessage(DEBUG, "accept success: %s:[%d], sockfd: %d", peerip.c_str(), peerport, sock);

            transervice(sock, peerip, peerport);
        }
    }
    // 5.提供服务
    void transervice(int sock, string &clientip, uint16_t clientport)
    {
        char inbuffer[1024];
        while (1)
        {
            ssize_t s = read(sock, inbuffer, sizeof(inbuffer) - 1);
            if (s > 0)
            {
                inbuffer[s] = 0;
                if (strcasecmp(inbuffer, "quit") == 0) // strcasecmp是忽略大小写比较的函数
                {
                    // 客户端输入退出
                    logMessage(DEBUG, "client quit -- %s[%d]", clientip.c_str(), clientport);
                    break;
                }
                logMessage(DEBUG, "trans before: %s[%d]>>> %s", clientip.c_str(), clientport, inbuffer);

                for (int i = 0; i < s; i++)
                {
                    if (isalpha(inbuffer[i]) && islower(inbuffer[i]))
                        inbuffer[i] = toupper(inbuffer[i]);
                }
                logMessage(DEBUG, "trans after: %s[%d]>>> %s", clientip.c_str(), clientport, inbuffer);

                write(sock, inbuffer, sizeof(inbuffer));
            }
            else if (s == 0)
            {
                logMessage(DEBUG, "client quit -- %s[%d]", clientip.c_str(), clientport);
                break;
            }
            else
            {
                logMessage(DEBUG, "%s[%d] - read: %s", clientip.c_str(), clientport, strerror(errno));
                break;
            }
            
        }
        close(sock);
        logMessage(DEBUG, "server close %d done", sock);
    }

private:
    uint16_t _port;
    string _ip;
    int _listensock;
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage:\n\t" << argv[0] << " port [ip]" << endl;
        exit(2);
    }
    uint16_t port = atoi(argv[1]);
    TcpServer svr(port);
    svr.init();
    svr.loop();

    return 0;
}