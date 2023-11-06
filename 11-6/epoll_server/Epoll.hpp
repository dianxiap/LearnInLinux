#pragma once

#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>

class Epoll
{
public:
    static const int gsize=256;
public:
    static int CreateEpoll()
    {
        int epfd=epoll_create(gsize);
        if(epfd>0)return epfd;
        exit(5);
    }
    static bool CtlEpoll(int epfd,int oper,int sock,uint32_t events)
    {
        struct epoll_event ev;
        ev.events=events;
        ev.data.fd=sock;
        int n=epoll_ctl(epfd,oper,sock,&ev);
        return n==0;
    }
    static int WaitEpoll(int epfd,struct epoll_event revs[],int num,int timeout)
    {
        //细节1：
        //细节2：
        return epoll_wait(epfd,revs,num,timeout);
    }
};