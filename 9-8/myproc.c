#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
  pid_t t=fork();
  if(t<0)
  {
    perror("fork error");
    return 1;
  }
  else if(t==0)
  {
    int cnt=5;
    while(cnt)
    {
       printf("这是子进程：pid: %d,ppid: %d ,cnt: %d\n",getpid(),getppid(),cnt);
       cnt--;
       sleep(1);
    }
  }
  else{
    while(1)
    {
       printf("这是父进程：pid: %d,ppid: %d\n",getpid(),getppid());
       sleep(1);
    }
  }
  return 0;
}
