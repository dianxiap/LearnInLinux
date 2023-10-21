#include "udp_server.hpp"
#include <memory>

static void usage(string proc)
{
    cout << "\nUsage: " << proc << " port \n"
         << endl;
}

// ./udp_server ip port
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
        exit(1);
    }
    // string ip = argv[1];
    uint16_t port = atoi(argv[1]);
    // 智能指针、RAII
    unique_ptr<UdpServer> svr(new UdpServer(port));

    svr->initServer();
    svr->start();

    return 0;
}