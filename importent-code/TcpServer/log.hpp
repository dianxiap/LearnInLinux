#pragma once
#include <cstdio>
#include <cstdarg>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <ctime>

#define DEBUG 0
#define NOTICE 1
#define WARINING 2
#define FATAL 3

const char *log_level[] = {"DEBUG", "NOTICE", "WARINING", "FATAL"};
// 打印日志函数
void logMessage(int level, const char *format, ...) // logMessage(DEBUG, "%d", 10);
{
    // 确保level等级是有效的
    assert(level >= DEBUG);
    assert(level <= FATAL);

    // 获取当前的使用者（环境变量）
    char *name = getenv("USER");

    char logInfo[1024];
    // 定义一个va_list类型的指针表示可变参数列表类型
    va_list ap;
    // 初始化此指针变量
    va_start(ap, format);
    // 将可变参数格式化输出到一个字符数组logInfo里
    vsnprintf(logInfo, sizeof(logInfo) - 1, format, ap);
    // 置空此指针变量
    va_end(ap);

    FILE *out = (level == FATAL) ? stderr : stdout;
    // 日志等级，打印日志的时间，打印日志的用户名
    fprintf(out, "%s | %u | %s | %s\n",
            log_level[level],
            (unsigned int)time(nullptr),
            name == nullptr ? "unknow" : name,
            logInfo);
}