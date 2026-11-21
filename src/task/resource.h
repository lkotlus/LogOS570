#ifndef RESOURCE_H
#define RESOURCE_H

#include "config.h"

struct task;

struct resource {
  int capacity;
  int available;
  struct task* waiters[LOGOS_MAX_PROCESSES];  // FIFO (oldest first)
  int waiter_count;
};

void resource_set_capacity(int id, int capacity);
int resource_acquire(int id, int count);
int resource_release(int id, int count);
struct resource* resource_get(int id);
void resource_release_all(struct task* task);
void resource_cancel_wait(struct task* task);

#endif
