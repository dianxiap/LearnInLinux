#include "TcpServer.hpp"
#include "Protocol.hpp"
#include <memory>

using namespace ns_tcpserver;
using namespace ns_protocol;

static void usage(const std::string &process)
{
    std::cout << "\nusage: " << process << " port\n"
              << std::endl;
}

// void debug(int sock)
// {
//     std::cout<<"我是测试服务，得到的sock："<<sock<<std::endl;
// }

static Response calculatorHelp(const Request &req)
{
    Response resp(0, 0);
    switch (req.op_)
    {
    case '+':
        resp.result_ = req.x_ + req.y_;
        break;
    case '-':
        resp.result_ = req.x_ - req.y_;
        break;
    case '*':
        resp.result_ = req.x_ * req.y_;
        break;
    case '/':
        if (0 == req.y_)
            resp.code_ = 1;
        else
            resp.result_ = req.x_ / req.y_;
        break;
    case '%':
        if (0 == req.y_)
            resp.code_ = 2;
        else
            resp.result_ = req.x_ % req.y_;
        break;
    default:
        resp.code_ = 3;
        break;
    }
    return resp;
}
void calculator(int sock)
{
    while (true)
    {
        std::string str = Recv(sock); // 收到了一个请求
        Request req;
        // 1.反序列化
        req.Deseriallized(str);
        // 2.对结构化的数据计算，返回结果
        Response resp = calculatorHelp(req);
        // 3.对计算结果进行序列化，然后返还客户端
        std::string respString = resp.Seriallize();
        Send(sock, respString);
    }
}

//.CalServer port
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
        exit(1);
    }
    
    std::unique_ptr<TcpServer> server(new TcpServer(atoi(argv[1])));
    server->BindService(calculator);
    server->start();
    // std::unique_ptr<TcpServer> server(new TcpServer(atoi(argv[1])));
    // server->BindService();
    // server->start();

    // Request req(123,456,'+');
    // std::string s=req.Seriallize();
    // std::cout<<s<<std::endl;

    // Request temp;
    // temp.Deseriallized(s);
    // std::cout<<temp.x_<<std::endl;
    // std::cout<<temp.op_<<std::endl;
    // std::cout<<temp.y_<<std::endl;
}