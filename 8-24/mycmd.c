#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
  char who[32];
  strcpy(who,getenv("USER"));

  if(strcpy(who,"root")==0)
  {
    printf("让他做任何事\n");
  }
  else{
    printf("你就是一个普通用户\n");
  }
  return 0;
}
