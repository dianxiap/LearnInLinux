#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc,char *argv[],char *env[])
{
  int i=0;
  for(;env[i];i++)
  {
    printf("env[%d]->%s\n",i,env[i]);
  }
  return 0;
}
