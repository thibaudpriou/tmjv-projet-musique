#include "common.h"
#include <math.h>

int
my_modulo(int x, int y)
{
  int res = fmod(x,y);
  if (res <0)
    res+=y;
  return res;
}
