#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void usage(std::string proc)
{
    std::cout << "\nUsage: " << proc << " serverIp serverPort\n"
              << std::endl;
}

// ./tcp_client targetIp targetPort
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
        exit(1);
    }
    std::string serverip = argv[1];
    uint16_t serverport = atoi(argv[2]);
    bool alive = false;
    int sock = 0;
    std::string line;
    while (true) // TODO
    {
        if (!alive)
        {
            sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0)
            {
                std::cerr << "socket error" << std::endl;
                exit(2);
            }
            // client 要不要bind呢？不需要显示的bind，但是一定是需要port
            // 需要让os自动进行port选择
            // 连接别人的能力！
            struct sockaddr_in server;
            memset(&server, 0, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_port = htons(serverport);
            server.sin_addr.s_addr = inet_addr(serverip.c_str());

            if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
            {
                std::cerr << "connect error" << std::endl;
                exit(3); // TODO
            }
            std::cout << "connect success" << std::endl;
            alive = true;
        }
        std::cout << "请输入# ";
        std::getline(std::cin, line);
        if (line == "quit")
            break;

        ssize_t s = send(sock, line.c_str(), line.size(), 0);
        if (s > 0)
        {
            char buffer[1024];
            ssize_t s = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (s > 0)
            {
                buffer[s] = 0;
                std::cout << "server 回显# " << buffer << std::endl;
            }
            else if (s == 0)
            {
                alive = false;
                close(sock);
            }
        }
        else
        {
            alive = false;
            close(sock);
        }
    }

    return 0;
}