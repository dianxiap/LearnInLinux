#include "EpollServer.hpp"
#include <memory>

using namespace std;
using namespace ns_epoll;

void change(string request)
{
    //完成业务逻辑
    cout<<"change: "<<request<<endl;
}

int main()
{
    unique_ptr<EpollServer> epoll_server(new EpollServer(change));
    epoll_server->start();

    return 0;
}