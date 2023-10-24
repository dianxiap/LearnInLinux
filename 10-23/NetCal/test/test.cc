#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

int main()
{
    int a = 10;
    int b = 20;
    char c = '+';

    Json::Value root;
    
    // kv模型
    root["aa"] = a;
    root["bb"] = b;
    root["op"] = c;

    Json::Value sub;
    sub["other"] = 200;
    sub["other1"] = "hello";

    root["sub"] = sub;

    // 序列化
    Json::StyledWriter writer;
    // Json::FastWriter writer;
    std::string s = writer.write(root);
    std::cout << s << std::endl;
}