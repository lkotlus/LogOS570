#include "logos.h"

// Even PIDs lock R0 then R1, odd PIDs lock R1 then R0. Sleep lets each task
// take the first lock before either ask for the second, so we deadlock on the
// first round every time.
int main() {
  int pid = getpid();
  int first = (pid % 2 == 0) ? 0 : 1;
  int second = 1 - first;

  for (int round = 1;; round++) {
    lock(first);
    printf("T%d: holding R%d, now wants R%d\n", pid, first, second);
    sleep(20);
    lock(second);
    printf("T%d: round %d, got both\n", pid, round);
    unlock(second);
    unlock(first);
  }
}

// If you change your mind, I'll be first in line
