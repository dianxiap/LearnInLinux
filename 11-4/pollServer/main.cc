#include "pollServer.hpp"
#include <memory>

int main()
{
    // 1. fd_set是一个固定大小位图，直接决定了select能同时关心的fd的个数是有上限的！
    // std::cout << sizeof(fd_set) * 8 << std::endl;

    std::unique_ptr<PollServer> svr(new PollServer());
    svr->Start();
    
    return 0;
}
// #include <poll.h>
// #include <unistd.h>
// #include <stdio.h>
// int main()
// {
//     struct pollfd poll_fd;
//     poll_fd.fd = 0;
//     poll_fd.events = POLLIN;

//     for (;;)
//     {
//         int ret = poll(&poll_fd, 1, 1000);
//         if (ret < 0)
//         {
//             perror("poll");
//             continue;
//         }
//         if (ret == 0)
//         {
//             printf("poll timeout\n");
//             continue;
//         }
//         if (poll_fd.revents == POLLIN)
//         {
//             char buf[1024] = {0};
//             read(0, buf, sizeof(buf) - 1);
//             printf("stdin:%s", buf);
//         }
//     }
// }