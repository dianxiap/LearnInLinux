#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "task.hpp"
using namespace std;

const int gnum = 3;
Task t;

class EndPoint
{
private:
    static int number;

public:
    pid_t _child_id;
    int _write_fd;
    std::string processname;

public:
    EndPoint(int id, int fd) : _child_id(id), _write_fd(fd)
    {
        // process-0[pid:fd]
        char namebuffer[64];
        snprintf(namebuffer, sizeof(namebuffer), "process-%d[%d:%d]", number++, _child_id, _write_fd);
        processname = namebuffer;
    }
    std::string name() const
    {
        return processname;
    }
    ~EndPoint()
    {
    }
};

int EndPoint::number = 0;

// 子进程要执行的方法
void WaitCommand()
{
    while (true)
    {
        int command = 0;
        int n = read(0, &command, sizeof(int));
        if (n == sizeof(int))
        {
            t.Execute(command);
        }
        else if (n == 0)
        {
            std::cout << "父进程让我退出，我就退出了: " << getpid() << std::endl;
            break;
        }
        else
        {
            break;
        }
    }
}

void createProcesses(vector<EndPoint> *end_points)
{
    vector<int> fds;
    for (int i = 0; i < gnum; i++)
    {
        // 1.1 创建管道
        int pipefd[2] = {0};
        int n = pipe(pipefd);
        assert(n == 0);
        (void)n;

        // 1.2 创建进程
        pid_t id = fork();
        assert(id != -1);
        // 一定是子进程
        if (id == 0)
        {
            for (auto &fd : fds)
                close(fd);

            // std::cout << getpid() << " 子进程关闭父进程对应的写端：";
            // for(auto &fd : fds)
            // {
            //     std::cout << fd << " ";
            //     close(fd);
            // }
            // std::cout << std::endl;

            // 1.3 关闭不要的fd
            close(pipefd[1]);
            // 我们期望，所有的子进程读取"指令"的时候，都从标准输入读取
            // 1.3.1 输入重定向，可以不做
            dup2(pipefd[0], 0);
            // 1.3.2 子进程开始等待获取命令
            WaitCommand();
            close(pipefd[0]);
            exit(0);
        }

        // 一定是父进程
        //  1.3 关闭不要的fd
        close(pipefd[0]);

        // 1.4 将新的子进程和他的管道写端，构建对象
        end_points->push_back(EndPoint(id, pipefd[1]));

        fds.push_back(pipefd[1]);
    }
}

int ShowBoard()
{
    std::cout << "##########################################" << std::endl;
    std::cout << "|   0. 执行日志任务   1. 执行数据库任务    |" << std::endl;
    std::cout << "|   2. 执行请求任务   3. 退出             |" << std::endl;
    std::cout << "##########################################" << std::endl;
    std::cout << "请选择# ";
    int command = 0;
    std::cin >> command;
    return command;
}

void ctrlProcess(const vector<EndPoint> &end_points)
{
    // 2.1 我们可以写成自动化的，也可以搞成交互式的
    int num = 0;
    int cnt = 0;
    while (true)
    {
        // 1. 选择任务
        int command = ShowBoard();
        if (command == 3)
            break;
        if (command < 0 || command > 2)
            continue;

        // 2. 选择进程
        int index = cnt++;
        cnt %= end_points.size();
        std::string name = end_points[index].name();
        std::cout << "选择了进程: " << name << " | 处理任务: " << command << std::endl;

        // 3. 下发任务
        write(end_points[index]._write_fd, &command, sizeof(command));

        sleep(1);
    }
}

void waitProcess(const vector<EndPoint> &end_points)
{
    // 1. 我们需要让子进程全部退出 --- 只需要让父进程关闭所有的write fd就可以了！
    // for(const auto &ep : end_points)
    // for(int end = end_points.size() - 1; end >= 0; end--)
    for (int end = 0; end < end_points.size(); end++)
    {
        std::cout << "父进程让子进程退出:" << end_points[end]._child_id << std::endl;
        close(end_points[end]._write_fd);

        waitpid(end_points[end]._child_id, nullptr, 0);
        std::cout << "父进程回收了子进程:" << end_points[end]._child_id << std::endl;
    }
    sleep(10);

    // 2. 父进程要回收子进程的僵尸状态
    // for(const auto &ep : end_points) waitpid(ep._child_id, nullptr, 0);
    // std::cout << "父进程回收了所有的子进程" << std::endl;
    // sleep(10);
}

// #define COMMAND_LOG 0
// #define COMMAND_MYSQL 1
// #define COMMAND_REQEUST 2
int main()
{
    vector<EndPoint> end_points;
    // 1. 先进行构建控制结构, 父进程写入，子进程读取 ， bug?
    createProcesses(&end_points);

    // 2. 我们的得到了什么？end_points
    ctrlProcess(end_points);

    // 3. 处理所有的退出问题
    waitProcess(end_points);
    return 0;
}