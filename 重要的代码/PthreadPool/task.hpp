#pragma once

#pragma once

#include <iostream>

// 任务类
class Task
{
public:
    Task(int x = 0, int y = 0, char op = 0)
        : _x(x), _y(y), _op(op)
    {
    }
    ~Task()
    {
    }

    // 处理任务的方法
    void Run()
    {
        int result = 0;
        switch (_op)
        {
        case '+':
            result = _x + _y;
            break;
        case '-':
            result = _x - _y;
            break;
        case '*':
            result = _x * _y;
            break;
        case '/':
            if (_y == 0)
            {
                std::cerr << "Error: div zero!" << std::endl;
                return;
            }
            else
            {
                result = _x / _y;
            }
            break;
        case '%':
            if (_y == 0)
            {
                std::cerr << "Error: mod zero!" << std::endl;
                return;
            }
            else
            {
                result = _x % _y;
            }
            break;
        default:
            std::cerr << "operation error!" << std::endl;
            return;
        }
        std::cout << "thread[" << pthread_self() << "]:" << _x << _op << _y << "=" << result << std::endl;
    }

private:
    int _x;
    int _y;
    char _op;
};
