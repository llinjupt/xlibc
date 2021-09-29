#include <stdio.h>
#include "test.h"

int main()
{
  int a = 0;

  inc(&a);
  printf("a:%d\n", a);

  return a;
}

