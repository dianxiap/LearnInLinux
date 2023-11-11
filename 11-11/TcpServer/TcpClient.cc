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

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage:\t\n"
             << argv[0] << " serverip serverport" << endl;
        exit(2);
    }
    string serverip = argv[1];
    uint16_t serverport = atoi(argv[2]);
    bool alive = false;
    int sock = 0;
    std::string line;

    while (1)
    {
        if (!alive)
        {
            sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0)
            {
                std::cerr << "socket error" << std::endl;
                exit(3);
            }

            struct sockaddr_in server;
            bzero(&server, sizeof server);
            server.sin_family = AF_INET;
            server.sin_port = htons(serverport);
            server.sin_addr.s_addr = inet_addr(serverip.c_str());

            if (connect(sock, (struct sockaddr *)&server, sizeof server) != 0)
            {
                std::cerr << "connect error" << std::endl;
                exit(4);
            }
            std::cout << "connect success" << std::endl;
            alive = true;
        }
        line.clear();
        cout<<"请输入# ";
        getline(cin,line);
        if(line=="quit")break;

        ssize_t s = send(sock, line.c_str(), line.size(), 0);
        if(s>0)
        {
            char buffer[1024];
            ssize_t ss = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if(ss>0)
            {
                buffer[ss]=0;
                std::cout << "server 回显# " << buffer << std::endl;
            }
            else if(ss==0)
            {
                alive=false;
                close(sock);
            }
        }
        else 
        {
            alive=false;
            close(sock);
        }
    }
    return 0;
}