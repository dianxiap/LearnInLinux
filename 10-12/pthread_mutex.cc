#include <iostream>
#include <thread>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <cstdio>
#include <assert.h>

using namespace std;
// 如果多线程访问同一个全局变量，并对它进行数据计算，多线程会互相影响吗？会

//全局的锁
// pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;
// int tickets = 10000; // 在并发访问的时候，导致了我们数据不一致的问题！

// void *getTickets(void *args)
// {
//     (void)args;
//     while(true)
//     {
//         pthread_mutex_lock(&mtx);
//         if(tickets > 0)
//         {
//             usleep(1000);
//             printf("%s: %d\n", (char*)args, tickets);
//             tickets--;
//             pthread_mutex_unlock(&mtx);
//         }
//         else{
//             pthread_mutex_unlock(&mtx);
//             break;
//         }
//     }
//     return nullptr;
// }

// int main()
// {
//     pthread_t t1,t2,t3;
//     // 多线程抢票的逻辑
//     pthread_create(&t1, nullptr, getTickets, (void*)"thread one");
//     pthread_create(&t2, nullptr, getTickets, (void*)"thread two");
//     pthread_create(&t3, nullptr, getTickets, (void*)"thread three");

//     pthread_join(t1, nullptr);
//     pthread_join(t2, nullptr);
//     pthread_join(t3, nullptr);
// }

int tickets = 10000; // 在并发访问的时候，导致了我们数据不一致的问题！临界资源

#define THREAD_NUM 600

class ThreadData
{
public:
    ThreadData(const std::string &n,pthread_mutex_t *pm):tname(n), pmtx(pm)
    {}
public:
    std::string tname;
    pthread_mutex_t *pmtx;
};

void *getTickets(void *args)
{
    // int myerrno = errno;
    ThreadData *td = (ThreadData*)args;
    while(true)
    {
        // 抢票逻辑
        int n = pthread_mutex_lock(td->pmtx);
        assert(n == 0);
        // 临界区
        if(tickets > 0) // 1. 判断的本质也是计算的一种
        {
            usleep(rand()%1500);
            printf("%s: %d\n", td->tname.c_str(), tickets);
            tickets--; // 2. 也可能出现问题
            n = pthread_mutex_unlock(td->pmtx);
            assert(n == 0);
        }
        else{
            n = pthread_mutex_unlock(td->pmtx);
            assert(n == 0);
            break;
        }
        
        // 抢完票，其实还需要后续的动作
        usleep(rand()%2000);
        //  errno = myerrno;
    }
    delete td;
    return nullptr;
}

int main()
{
    time_t start = time(nullptr);

    pthread_mutex_t mtx;
    pthread_mutex_init(&mtx, nullptr);

    srand((unsigned long)time(nullptr) ^ getpid() ^ 0x147);
    pthread_t t[THREAD_NUM];
    // 多线程抢票的逻辑
    for(int i = 0; i < THREAD_NUM; i++)
    {
        std::string name = "thread ";
        name += std::to_string(i+1);
        
        ThreadData *td = new ThreadData(name, &mtx);
        
        pthread_create(t + i, nullptr, getTickets, (void*)td);
        
    }

    for(int i = 0; i < THREAD_NUM; i++)
    {
        pthread_join(t[i], nullptr);
    }

    pthread_mutex_destroy(&mtx);

    time_t end = time(nullptr);

    cout << "cast: " << (int)(end - start) << "S" << endl;
}