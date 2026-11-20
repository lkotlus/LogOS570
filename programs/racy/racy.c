#include "logos.h"

// An ABBA alternative: opposite lock orders without the sleep. So we only have deadlock
// when a timer tick lands inside the defined WINDOW and the other task takes its first lock.
// We run perfectly fine for a random amount of time, and then freeze. As the window grows
// larger relative to work, we deadlock sooner.
#define WINDOW 2000
#define WORK 20000

int main() {
  int pid = getpid();
  int first = (pid % 2 == 0) ? 0 : 1;
  int second = 1 - first;

  for (unsigned int round = 1;; round++) {
    lock(first);
    spin(WINDOW);
    lock(second);
    spin(WORK);
    unlock(second);
    unlock(first);

    if (round % 50 == 0) {
      printf("T%d: %u rounds\n", pid, round);
    }
  }
}

// Honey I'm still free, take a chance on me
