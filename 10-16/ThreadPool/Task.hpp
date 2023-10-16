#pragma once

#include <iostream>
#include <functional>
#include <string>
#include "log.hpp"

typedef std::function<int (int,int)> func_t; 

class Task
{
public:
    Task(){}
    Task(int x,int y,func_t func)
    :_x(x)
    ,_y(y)
    ,_func(func)
    {}

    void operator()(const string &name)
    {
        logmessage(WARNING,"%s处理完成：%d+%d=%d | %s | %d",name.c_str(),_x,_y,_func(_x,_y),__FILE__,__LINE__);
    }
public:
    int _x;
    int _y;
    func_t _func;
};