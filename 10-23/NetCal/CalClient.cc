#include <iostream>
#include "Sock.hpp"
#include "Protocol.hpp"

using namespace ns_protocol;

static void Usage(const std::string &process)
{
    std::cout << "\nUsage: " << process << " serverIp serverPort\n"
              << std::endl;
}

// ./client server_ip server_port
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    std::string server_ip = argv[1];
    uint16_t server_port = atoi(argv[2]);
    Sock sock;
    int sockfd = sock.Socket();
    if (!sock.Connect(sockfd, server_ip, server_port))
    {
        std::cerr << "Connect error" << std::endl;
        exit(2);
    }

    bool quit = false;
    std::string buffer;
    while (!quit)
    {
        // 1. 获取需求
        Request req;
        std::cout << "Please Enter # ";
        std::cin >> req.x_ >> req.op_ >> req.y_;
        // 2. 序列化
        std::string s = req.Serialize();
        // 3. 添加长度报头
        s = Encode(s);
        // 4. 发送给服务端
        Send(sockfd, s);

        // 5. 正常读取
        while (true)
        {
            bool res = Recv(sockfd, &buffer);
            if (!res)
            {
                quit = true;
                break;
            }
            std::string package = Decode(buffer);
            if (package.empty())
                continue;
            Response resp;
            resp.Deserialized(package);
            std::string err;
            switch (resp.code_)
            {
            case 1:
                err = "除0错误";
                break;
            case 2:
                err = "模0错误";
                break;
            case 3:
                err = "非法操作";
                break;
            default:
                std::cout << resp.x_ << resp.op_ << resp.y_ << " = " << resp.result_ << " [success]" << std::endl;
                break;
            }
            if (!err.empty())
                std::cerr << err << std::endl;
            // sleep(1);
            break;
        }
    }
    close(sockfd);
    return 0;
}
