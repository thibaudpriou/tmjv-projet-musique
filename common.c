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

double
mean(double *array, const int size)
{
    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
     sum += array[i];
    }
    return ((double) sum)/size;
}

double
meanInt(int *array, const int size)
{
    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
     sum += array[i];
    }
    return ((double) sum)/size;
}
