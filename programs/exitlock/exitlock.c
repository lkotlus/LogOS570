#include "logos.h"

// First instance of the process takes R0 and R1 without unlocking before
// exit. The next instance should be able to take R0 and R1 if exit works
// properly.
int main() {
  int pid = getpid();

  if (pid == 0) {
    lock(0);
    lock(1);
    printf("T0: holding R0 and R1, exiting without unlocking\n");
    sleep(50);
    return 7;
  }

  sleep(10);
  lock(0);
  lock(1);
  printf("T%d: got R0 and R1 after T0 exited\n", pid);
  unlock(1);
  unlock(0);
  return pid;
}
