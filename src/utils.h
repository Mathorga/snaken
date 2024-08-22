#include <time.h>

long long random (int range) {
  struct timespec t ;
  clock_gettime(CLOCK_REALTIME , &t);
  return (t.tv_sec * 1000 + (t.tv_nsec + 500000) / 1000000) % range;
}