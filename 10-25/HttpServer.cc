#include <iostream>
#include <memory>
#include <cassert>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "HttpServer.hpp"
#include "Usage.hpp"
#include "Util.hpp"

// 一般http都要有自己的web根目录
#define ROOT "./wwwroot" // ./wwwroot/index.html
// 如果客户端只请求了一个/,我们返回默认首页
#define HOMEPAGE "index.html"

void HandlerHttpRequest(int sockfd)
{
    // 1. 读取请求 for test
    char buffer[10240];
    ssize_t s = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (s > 0)
    {
        buffer[s] = 0;
        // std::cout << buffer << "--------------------\n" << std::endl;
    }

    std::vector<std::string> vline;
    Util::cutString(buffer, "\n", &vline);

    std::vector<std::string> vblock;
    Util::cutString(vline[0], " ", &vblock);

    std::string file = vblock[1];
    std::string target = ROOT;

    if (file == "/")
        file = "/index.html";
    target += file;
    std::cout << target << std::endl;

    std::string content;
    std::ifstream in(target);
    if (in.is_open())
    {
        std::string line;
        while (std::getline(in, line))
        {
            content += line;
        }
        in.close();
    }

    std::string HttpResponse;
    if (content.empty())
        HttpResponse = "HTTP/1.1 404 NotFound\r\n";
    else
        HttpResponse = "HTTP/1.1 200 OK\r\n";
    HttpResponse += "\r\n";
    HttpResponse += content;
    // std::cout << "####start################" << std::endl;
    // for(auto &iter : vblock)
    // {
    //     std::cout << "---" << iter << "\n" << std::endl;
    // }
    // std::cout << "#####end###############" << std::endl;

    // 2. 试着构建一个http的响应
    send(sockfd, HttpResponse.c_str(), HttpResponse.size(), 0);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }
    std::unique_ptr<HttpServer> httpserver(new HttpServer(atoi(argv[1]), HandlerHttpRequest));
    httpserver->Start();
    return 0;
}