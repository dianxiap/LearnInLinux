#include "TcpServer.hpp"
#include "Protocol.hpp"
#include "Daemon.hpp"
#include <memory>
#include <signal.h>

using namespace ns_tcpserver;
using namespace ns_protocol;

static void Usage(const std::string &process)
{
    std::cout << "\nUsage: " << process << " port\n"
              << std::endl;
}

static Response calculatorHelper(const Request &req)
{
    Response resp(0, 0, req.x_, req.y_, req.op_);
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
    std::string inbuffer;
    while (true)
    {
        // 1. 读取成功
        bool res = Recv(sock, &inbuffer); // 在这里我们读到了一个请求
        if (!res)
            break;
        // 2. 协议解析，保证得到一个完整的报文
        std::string package = Decode(inbuffer);
        if (package.empty())
            continue;
        logMessage(NORMAL, "%s", package.c_str());
        // 3. 保证该报文是一个完整的报文
        Request req;
        // 4. 反序列化，字节流 -> 结构化
        req.Deserialized(package); // 反序列化
        // 5. 业务逻辑
        Response resp = calculatorHelper(req);
        // 6. 序列化
        std::string respString = resp.Serialize(); // 对计算结果进行序列化
        // 7. 添加长度信息，形成一个完整的报文
        // "length\r\ncode result\r\n"
        respString = Encode(respString);
        // std::cout << "encode: respString: " << respString << std::endl;
        // 8. send这里我们暂时先这样写，多路转接的时候，我们再来谈发送的问题
        Send(sock, respString);
    }
}

// ./CalServer port
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    // 一般经验：server在编写的时候，要有较为严谨性的判断逻辑
    // 一般服务器，都是要忽略SIGPIPE信号的，防止在运行中出现非法写入的问题！
    // signal(SIGPIPE, SIG_IGN);
    MyDaemon();
    std::unique_ptr<TcpServer> server(new TcpServer(atoi(argv[1])));
    server->BindService(calculator);
    server->Start();
    // Request req(123, 456, '+');
    // std::string s = req.Serialize();
    // std::cout << s << std::endl;

    // Request temp;
    // temp.Deserialized(s);
    // std::cout << temp.x_ << std::endl;
    // std::cout << temp.op_ << std::endl;
    // std::cout << temp.y_ << std::endl;
    return 0;
}
