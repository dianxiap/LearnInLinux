#include "common.hpp"

int main()
{
    //1.获取管道文件
    int fd=open(ipcPath.c_str(),O_WRONLY);
    if(fd<0)
    {
        perror("open");
        exit(1);
    }
    //2.ipc（通信）过程
    string buffer;
    while(true)
    {
        cout<<"pless enter message line :> ";
        getline(cin,buffer);
        write(fd,buffer.c_str(),buffer.size());
    }
    //3.关闭
    close(fd);
    return 0;

}