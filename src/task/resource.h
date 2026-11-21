#ifndef RESOURCE_H
#define RESOURCE_H

#include "config.h"

struct task;

// Single instance resource (mutex)
struct resource {
  struct task* owner;  // 0 when free
  struct task* waiters[LOGOS_MAX_PROCESSES];  // FIFO (oldest first)
  int waiter_count;
};

int resource_lock(int id);
int resource_unlock(int id);
struct resource* resource_get(int id);
void resource_release_all(struct task* task);
void resource_cancel_wait(struct task* task);

#endif
