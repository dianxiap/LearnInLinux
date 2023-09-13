#include <stdio.h>
#include <stdlib.h>

int main()
{
  printf("hello linux!\n");
  printf("MYVAL=%s\n",getenv("MYVAL"));
  return 0;
}
