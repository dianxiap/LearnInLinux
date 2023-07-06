#include "ProcessBar.h"
#include <unistd.h>

// 函数指针类型
typedef void(*callback_t)(int); 

void download(callback_t cb)
{
  int total=1000;
  int cur=0;
  while(cur<=total)
  {
    usleep(50000);
    int rate=cur*100/total;

    cb(rate);

    cur+=10;

  }
  printf("\n");
}

int main()
{
  printf("download 1: \n");
  download(processbar);

  printf("download 2: \n");
  download(processbar);
  printf("download 3: \n");
  download(processbar);
  printf("download 4: \n");
  download(processbar);
}
