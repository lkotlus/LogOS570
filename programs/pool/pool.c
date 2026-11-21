#include "logos.h"

// Pool deadlock: R0 is a pool of 3 units.
int main() {
  int pid = getpid();

  acquire(0, 1);
  printf("T%d: holding 1 unit of R0, wants 1 more\n", pid);
  sleep(20);

  acquire(0, 1);
  printf("T%d: got 2 units (no deadlock?)\n", pid);
  release(0, 2);
  return 0;
}
