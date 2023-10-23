#include <iostream>
#include "Sock.hpp"
#include "Protocol.hpp"

using namespace ns_protocol;

static void usage(const std::string &process)
{
    std::cout << "\nusage: " << process << " serverip serverport\n"
              << std::endl;
}

//./client server_ip server_port
int main(int argc,char*argv[])
{
    if(argc!=3)
    {
        usage(argv[0]);
        exit(1);
    }

    std::string server_ip=argv[1];
    uint16_t server_port=atoi(argv[2]);
    Sock sock;
    int sockfd=sock.Socket();
    if(!sock.Connect(sockfd,server_ip,server_port))
    {
        std::cerr<<"connect error"<<std::endl;
        exit(2);
    }

    while(true)
    {
        Request req(10,20,'+');
        std:: string s=req.Seriallize();
        //发给服务端进行处理
        Send(sockfd,s);
        //返回结果
        std::string r=Recv(sockfd);
        Response resp;
        resp.Deseriallized(r);
        std::cout<<"code: "<<resp.code_<<std::endl;
        std::cout<<"result: "<<resp.result_<<std::endl;
        sleep(1);
    }
    return 0;
}