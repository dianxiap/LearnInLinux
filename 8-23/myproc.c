#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() 
{
  while(1)
  {
    printf("i am process, pid: %d\n", getpid());
    sleep(1);
  }
//  pid_t id=fork();
//
//  if(id==0)
//  {
//    int cnt=5;
//    while(cnt)
//    {
//      printf("i am child, pid: %d, ppid: %d, cnt: %d\n",getpid(),getppid(),cnt);
//      cnt--;
//      sleep(1);
//    }
//    exit(0);
//  }
//  else
//  {
//    while(1)
//    {
//      printf("i am father, pid: %d, ppid: %d\n",getpid(),getppid());
//      sleep(1);
//    }
//  }
//
}
