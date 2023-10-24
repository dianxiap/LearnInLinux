#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <jsoncpp/json/json.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace ns_protocol
{
// #define MYSELF 0

#define SPACE " "
#define SPACE_LEN strlen(SPACE)
#define SEP "\r\n"
#define SEP_LEN strlen(SEP) // 不能是sizeof！

    class Request
    {
    public:
        // 1. 自主实现 "length\r\nx_ op_ y_\r\n"
        // 2. 使用现成的方案
        std::string Serialize()
        {
#ifdef MYSELF
            std::string str;
            str = std::to_string(x_);
            str += SPACE;
            str += op_; // TODO
            str += SPACE;
            str += std::to_string(y_);
            return str;
#else
            Json::Value root;
            root["x"] = x_;
            root["y"] = y_;
            root["op"] = op_;
            Json::FastWriter writer;
            return writer.write(root);
#endif
        }
        // "x_ op_ y_"
        // "1234 + 5678"
        bool Deserialized(const std::string &str)
        {
#ifdef MYSELF
            std::size_t left = str.find(SPACE);
            if (left == std::string::npos)
                return false;
            std::size_t right = str.rfind(SPACE);
            if (right == std::string::npos)
                return false;
            x_ = atoi(str.substr(0, left).c_str());
            y_ = atoi(str.substr(right + SPACE_LEN).c_str());
            if (left + SPACE_LEN > str.size())
                return false;
            else
                op_ = str[left + SPACE_LEN];
            return true;
#else
            Json::Value root;
            Json::Reader reader;
            // 反序列化
            reader.parse(str, root);
            x_ = root["x"].asInt();
            y_ = root["y"].asInt();
            op_ = root["op"].asInt();
            return true;
#endif
        }

    public:
        Request()
        {
        }
        Request(int x, int y, char op) : x_(x), y_(y), op_(op)
        {
        }
        ~Request() {}

    public:
        // 约定
        // x_ op y_ ? y_ op x_?
        int x_;   // 是什么？
        int y_;   // 是什么？
        char op_; // '+' '-' '*' '/' '%'
    };

    class Response
    {
    public:
        // "code_ result_"
        std::string Serialize()
        {
#ifdef MYSELF
            std::string s;
            s = std::to_string(code_);
            s += SPACE;
            s += std::to_string(result_);

            return s;
#else
            Json::Value root;
            root["code"] = code_;
            root["result"] = result_;
            root["xx"] = x_;
            root["yy"] = y_;
            root["zz"] = op_;
            Json::FastWriter writer;
            return writer.write(root);
#endif
        }
        // "111 100"
        bool Deserialized(const std::string &s)
        {
#ifdef MYSELF
            std::size_t pos = s.find(SPACE);
            if (pos == std::string::npos)
                return false;
            code_ = atoi(s.substr(0, pos).c_str());
            result_ = atoi(s.substr(pos + SPACE_LEN).c_str());
            return true;
#else
            Json::Value root;
            Json::Reader reader;
            reader.parse(s, root);
            code_ = root["code"].asInt();
            result_ = root["result"].asInt();
            x_ =  root["xx"].asInt();
            y_ =  root["yy"].asInt();
            op_ =  root["zz"].asInt();
            return true;
#endif
        }

    public:
        Response()
        {
        }
        Response(int result, int code, int x, int y, char op) 
        : result_(result), code_(code), x_(x), y_(y), op_(op)
        {
        }
        ~Response() {}

    public:
        // 约定！
        // result_? code_? code_ 0? 1?2?3?
        int result_; // 计算结果
        int code_;   // 计算结果的状态码

        int x_;
        int y_;
        char op_;
    };

    // 临时方案
    // 调整方案2： 我们期望，你必须给我返回一个完整的报文
    bool Recv(int sock, std::string *out)
    {
        // UDP是面向数据报:
        // TCP 面向字节流的:
        // recv ： 你怎么保证，你读到的inbuffer，是一个完整完善的请求呢？不能保证
        // "1234 + 5678" : 1234 +
        // "1234 + 5678" : 1234 + 5678 123+99
        // "1234 "
        // 必须是："1234 + 5678"
        // 单纯的recv是无法解决这个问题的，需要对协议进一步定制！
        char buffer[1024];
        ssize_t s = recv(sock, buffer, sizeof(buffer)-1, 0); // 9\r\n123+789\r\n
        if (s > 0)
        {
            buffer[s] = 0;
            *out += buffer;
        }
        else if (s == 0)
        {
            // std::cout << "client quit" << std::endl;
            return false;
        }
        else
        {
            // std::cout << "recv error" << std::endl;
            return false;
        }
        return true;
    }

    void Send(int sock, const std::string str)
    {
        // std::cout << "sent in" << std::endl;
        int n = send(sock, str.c_str(), str.size(), 0);
        if (n < 0)
            std::cout << "send error" << std::endl;
    }

    // 提取报文中的字符串内容
    // "length\r\nx_ op_ y_\r\n..." // 10\r\nabc
    // "x_ op_ y_\r\n length\r\nXXX\r\n"
    std::string Decode(std::string &buffer)
    {
        std::size_t pos = buffer.find(SEP);
        if(pos == std::string::npos) return "";
        int size = atoi(buffer.substr(0, pos).c_str());
        int surplus = buffer.size() - pos - 2*SEP_LEN;
        if(surplus >= size)
        {
            //至少具有一个合法完整的报文, 可以动手提取了
            buffer.erase(0, pos+SEP_LEN);
            std::string s = buffer.substr(0, size);
            buffer.erase(0, size + SEP_LEN);
            return s;
        }
        else
        {
            return "";
        }
    }

    // 给报文添加长度信息
    // "XXXXXX"
    // "123\r\nXXXXXX\r\n"
    std::string Encode(std::string &s)
    {
        std::string new_package = std::to_string(s.size());
        new_package += SEP;
        new_package += s;
        new_package += SEP;
        return new_package;
    }

}