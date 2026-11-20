#include "logos.h"

// Control program, no deadlocking
int main() {
  int pid = getpid();

  for (int round = 1;; round++) {
    lock(0);
    sleep(10);
    lock(1);
    printf("T%d: round %d, holding R0 and R1\n", pid, round);
    unlock(1);
    unlock(0);
    sleep(5);
  }
}
