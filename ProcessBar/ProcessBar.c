#include "ProcessBar.h"
#include <string.h>
#include <unistd.h>

const char* lable="|/-\\";
char bar[NUM];

#define GREEN "\033[0;32;32m"
#define NONE "\033[m"

//v2:如何被调用的
void processbar(int rate)
{
  if(rate<0||rate>100)return ;
  
  int len=strlen(lable);
  printf(GREEN"[%-100s]"NONE"[%d%%][%c]\r",bar,rate,lable[rate%len]);
  fflush(stdout);
  bar[rate++]=BODY;
  if(rate<100)bar[rate]=RIGHT;

}

void init()
{
  memset(bar,'\0',sizeof(bar));
}

//v1:普通版
//void processbar(int speed)
//{
//  char bar[NUM];
//  memset(bar ,'\0',sizeof(bar));
//
//  int len=strlen(lable);
//
//  int cnt=0;
//  while(cnt<=TOP)
//  {
//    //%r回车，%n回车换行
//    printf("[%-100s][%d%%][%c]\r",bar,cnt,lable[cnt%len]);
//    //刷新缓冲区
//    fflush(stdout);
//    bar[cnt++]=BODY;
//    if(cnt < 100) bar[cnt]=RIGHT;
//    usleep(speed);
//  }
//
//  printf("\n");
//
//}

