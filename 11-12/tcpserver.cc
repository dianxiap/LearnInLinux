#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

int main()
{
    // 创建套接字
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0)
    {
        cerr << "socket error!" << endl;
        return 1;
    }
    // 绑定
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(8081);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listen_sock, (struct sockaddr *)&local, sizeof(local)) < 0)
    {
        cerr << "bind error!" << endl;
        return 2;
    }
    // 监听
    if (listen(listen_sock, 5) < 0)
    {
        cerr << "listen error!" << endl;
        return 3;
    }
    // 启动服务器
    struct sockaddr peer;
    memset(&peer, 0, sizeof(peer));
    socklen_t len = sizeof(peer);
    for (;;)
    {
        int sock = accept(listen_sock, (struct sockaddr *)&peer, &len);
        if (sock < 0)
        {
            cerr << "accept error!" << endl;
            continue;
        }
        if (fork() == 0)
        { // 爸爸进程
            close(listen_sock);
            if (fork() > 0)
            { // 爸爸进程
                exit(0);
            }
            // 孙子进程
            char buffer[1024];
            recv(sock, buffer, sizeof(buffer), 0); // 读取HTTP请求
            cout << "--------------------------http request begin--------------------------" << endl;
            cout << buffer << endl;
            cout << "---------------------------http request end---------------------------" << endl;

            // 构建HTTP响应
            string status_line = "http/1.1 307 Temporary Redirect\n";     // 状态行
            string response_header = "Location: https://www.csdn.net/\n"; // 响应报头
            string blank = "\n";                                          // 空行
            string response = status_line + response_header + blank;      // 响应报文

            // 响应HTTP请求
            send(sock, response.c_str(), response.size(), 0);

            close(sock);
            exit(0);
        }
        // 爷爷进程
        close(sock);
        waitpid(-1, nullptr, 0); // 等待爸爸进程
    }
    return 0;
}
