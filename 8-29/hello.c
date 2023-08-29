#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int g_unval;
int g_val=100;

int main(int argc,char *argv[],char *env[])
{
  printf("code addr: %p\n",main);
  printf("init global addr: %p\n",&g_val);
  printf("uninit global addr: %p\n",&g_unval);

  char* heap_mem=(char*)malloc(10);

  printf("heap addr:%p\n",heap_mem);
  printf("stack addr:%p\n",&heap_mem);

  for(int i=0;i<argc;i++)
  {
    printf("argv[%d]:%p\n",i,argv[i]);
  }
  for(int i=0;env[i];i++)
  {
    printf("env[%d]:%p\n",i,env[i]);
  }
  return 0;
}
