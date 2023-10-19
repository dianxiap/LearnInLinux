#include "udp_server.hpp"
#include <memory>

static void usage(string proc)
{
    cout<<"\nUsage: "<<proc<<" ip port \n"<<endl;
}
// ./udp_server ip port
int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        usage(argv[0]);
        exit(1);
    }
    string ip=argv[1];
    uint16_t port=stoi(argv[2]);
    unique_ptr<UdpServer> svr(new UdpServer(port,ip));

    svr->initServer();
    svr->start();
       
    return 0;
}