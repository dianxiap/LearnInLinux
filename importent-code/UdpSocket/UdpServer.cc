#include "UdpServer.hpp"
#include "Err.hpp"
#include <memory>
#include <string>

using namespace ns_server;
using namespace std;

int main(int argc,char* argv[])
{
    if(argc!=2)
    {
        cout<<"Usage:\n\t"<<argv[0]<<" port\n"<<endl;
    }
    uint16_t port=atoi(argv[1]);

    unique_ptr<UdpServer> pus(new UdpServer(port));
    pus->Start();
}