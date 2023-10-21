#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <memory>
#include "thread.hpp"

using namespace std;

// 无论是多线程读还是写，用的sock都是一个，sock代表就是文件，UDP是全双工的 -> 可以同时收发而不受干扰

uint16_t serverport = 0;
string serverip;

static void usage(string proc)
{
    cout << "\nusage: " << proc << " serverip serverport\n"
         << endl;
}

static void *udpsend(void *args)
{
    int sock = *(int *)((ThreadData *)args)->args_;
    string name = ((ThreadData *)args)->name_;

    string message;
    // 要给谁发的信息
    struct sockaddr_in server;
    memset(&server, 0, sizeof server);
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    server.sin_addr.s_addr = inet_addr(serverip.c_str());

    while (true)
    {
        cerr<< "请输入你的信息# ";
        getline(cin, message);
        if (message == "quit")
            break;

        // 当client首次发送消息给服务器的时候，os会自动给client bind他的ip和port
        sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr *)&server, sizeof(server));
    }
    return nullptr;
}

static void *udprecv(void *args)
{
    int sock = *(int *)((ThreadData *)args)->args_;
    string name = ((ThreadData *)args)->name_;

    char buffer[1014];
    while (true)
    {
        memset(buffer,0,sizeof buffer);
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        ssize_t s = recvfrom(sock, buffer, sizeof buffer, 0, (struct sockaddr *)&temp, &len);

        if (s > 0)
        {
            buffer[s] = 0;
            cout << buffer << endl;
        }
    }
}

//./udp_client 127.0.0.1 8080
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
        exit(1);
    }
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        cerr << "socket error" << endl;
        exit(2);
    }

    serverport = atoi(argv[2]);
    serverip = argv[1];

    unique_ptr<Thread> sender(new Thread(1, udpsend, (void *)&sock));
    unique_ptr<Thread> recver(new Thread(1, udprecv, (void *)&sock));

    sender->start();
    recver->start();

    sender->join();
    recver->join();

    close(sock);

    // client不需要显示bind指定的port，系统自己随机选择bind，
    //  要发送的信息
    //  string message;
    //  // 要给谁发的信息
    //  struct sockaddr_in server;
    //  memset(&server,0,sizeof server);
    //  server.sin_family=AF_INET;
    //  server.sin_port=htons(atoi(argv[2]));
    //  server.sin_addr.s_addr=inet_addr(argv[1]);

    // char buffer[1024];
    // while(true)
    // {

    //     cout<<"请输入你的信息# ";
    //     getline(cin,message);
    //     if(message=="quit")break;

    //     // 当client首次发送消息给服务器的时候，os会自动给client bind他的ip和port
    //     sendto(sock,message.c_str(),message.size(),0,(struct sockaddr*)&server,sizeof(server));

    //     struct sockaddr_in temp;
    //     socklen_t len=sizeof(temp);
    //     ssize_t s=recvfrom(sock,buffer,sizeof buffer,0,(struct sockaddr*)&temp,&len);

    //     if(s>0)
    //     {
    //         buffer[s]=0;
    //         cout<<"server echo# "<<buffer<<endl;
    //     }
    // }
    // close(sock);
    return 0;
}