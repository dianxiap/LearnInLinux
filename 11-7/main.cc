#include "TcpServer.hpp"
#include <memory>

int main()
{
    std::unique_ptr<TcpServer> svr(new TcpServer());
    svr->Dispather();

    return 0;
}