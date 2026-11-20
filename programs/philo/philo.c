#include "logos.h"

// Dining philosophers! Imagine five philosophers who all eat at a table with forks
// on their left and right. The forks are resources R0 through R4. The ith philosopher
// takes fork i (his left-hand fork), waits, then takes fork i+1 (his right-hand fork).
// Everyone gras their left fork at the start, so we deadlock in a 5-task cycle on 
// the first round.
#define PHILOSOPHERS 5

int main() {
  int pid = getpid();
  int left = pid;
  int right = (pid + 1) % PHILOSOPHERS;

  for (int meal = 1;; meal++) {
    lock(left);
    printf("P%d: took fork %d, wants fork %d\n", pid, left, right);
    sleep(20);
    lock(right);
    printf("P%d: eating (meal %d)\n", pid, meal);
    sleep(30);
    unlock(right);
    unlock(left);
    sleep(30 + pid * 7);
  }
}
