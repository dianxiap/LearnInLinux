// 协议
#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>

namespace ns_protocol
{
#define MYSELF 1
#define SPACE " "
#define SPACE_LEN strlen(SPACE)
    // 结构化的数据
    class Request
    {
    public:
        // 序列化：结构体 -> 字符串 "x_ op_ y_"
        std::string Seriallize()
        {
#ifdef MYSELF
            // 1.自主实现
            std::string str;
            str = std::to_string(x_);
            str += SPACE;
            str += op_;
            str += SPACE;
            str += std::to_string(y_);
            return str;
#else
            // 2.现成的方案
            std::cout << "TODO" << endl;
#endif
        }
        // 反序列化：字符串 -> 结构体
        bool Deseriallized(const std::string &str)
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
#else
            std::cout << "TODO" << endl;
#endif
        }

    public:
        Request() {}
        Request(int x, int y, char op)
            : x_(x), y_(y), op_(op)
        {
        }
        ~Request() {}

    public:
        int x_;
        int y_;
        char op_;
    };

    class Response
    {
    public:
        //"code_ result_"
        // 序列化：结构体 -> 字符串
        std::string Seriallize()
        {
#ifdef MYSELF
            std::string s;
            s = std::to_string(code_);
            s += SPACE;
            s += std::to_string(result_);
#else
            std::cout << "TODO" << endl;
#endif
        }
        // 反序列化：字符串 -> 结构体
        bool Deseriallized(const std::string &s)
        {
#ifdef MYSELF
            std::size_t pos = s.find(SPACE);
            if (pos == std::string::npos)
                return false;
            code_ = atoi(s.substr(0, pos).c_str());
            result_ = atoi(s.substr(pos + SPACE_LEN).c_str());
            return true;
#else
            std::cout << "TODO" << endl;
#endif
        }

    public:
        Response() {}
        Response(int result, int code)
            : result_(result), code_(code)
        {
        }
        ~Response() {}

    public:
        int result_; // 计算结果
        int code_;   // 计算结果的状态码
    };

    std::string Recv(int sock)
    {
        char inbuffer[1024];
        ssize_t s = recv(sock, inbuffer, sizeof inbuffer, 0);
        if (s > 0)
            return inbuffer;
    }

    void Send(int sock, const std::string str)
    {
        send(sock, str.c_str(), str.size(), 0);
    }
}