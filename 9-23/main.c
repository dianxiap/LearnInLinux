#include <stdio.h>
#include "add.h"
#include "print.h"

int main()
{
  int res=Add(1,2);
  Print("Hello world!\n");
  printf("%d\n",res);

  return 0;
}
