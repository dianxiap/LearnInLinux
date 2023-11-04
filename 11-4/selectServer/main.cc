#include "selectServer.hpp"
using namespace std;

int main()
{
    //1.fd_set是一个固定大小的位图，直接决定了select能同时关心的fd的个数是有限的
    cout<<sizeof(fd_set)*8<<endl;
    // while (1)
    // {
    //     cout << "time: " << (unsigned long)time(nullptr) << endl;
    //     struct timeval currtime = {0, 0};
    //     int n = gettimeofday(&currtime, nullptr);
    //     assert(n == 0);
    //     (void)n;

    //     cout << "gettimeofday: " << currtime.tv_sec << "." << currtime.tv_usec << endl;
    //     sleep(1);
    // }
    return 0;
}