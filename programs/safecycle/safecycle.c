#include "logos.h"

// Cycle without a deadlock due to resource pools.
int main() {
  int pid = getpid();

  if (pid == 0) {
    acquire(0, 1);
    sleep(20);
    printf("T0: holding 1 of R0, waiting for R1\n");
    lock(1);
    printf("T0: got R1, finishing\n");
    unlock(1);
    release(0, 1);
  } else if (pid == 1) {
    lock(1);
    sleep(20);
    printf("T1: holding R1, waiting for 1 of R0\n");
    acquire(0, 1);
    printf("T1: got R0, finishing\n");
    release(0, 1);
    unlock(1);
  } else {
    acquire(0, 1);
    sleep(100);  // The cycle exists the whole time T2 sleeps
    printf("T2: releasing my unit of R0, the cycle resolves\n");
    release(0, 1);
  }
  return pid;
}
