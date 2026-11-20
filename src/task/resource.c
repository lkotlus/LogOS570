#include "resource.h"

#include "kernel.h"
#include "process.h"
#include "status.h"
#include "task.h"

// Only touched by syscalls, so all operations are atomic
static struct resource resources[LOGOS_MAX_RESOURCES];

static int resource_id_valid(int id) {
  return id >= 0 && id < LOGOS_MAX_RESOURCES;
}

struct resource* resource_get(int id) {
  return resource_id_valid(id) ? &resources[id] : 0;
}

static void resource_trace(struct task* task, const char* what, int id) {
#if LOGOS_TRACE_RESOURCES
  print("  [T");
  print_int(task->process->id);
  print(" ");
  print(what);
  print(" R");
  print_int(id);
  print("]\n");
#endif
}

// Take resource or block until handed over
int resource_lock(int id) {
  struct task* task = task_current();
  struct resource* res = resource_get(id);

  if (!res) {
    return -EINVARG;
  }
  if (res->owner == task) {
    return -EISTKN;  // Not recursive
  }

  if (!res->owner) {
    res->owner = task;
    resource_trace(task, "got", id);
    return 0;
  }

  // Held by someone else, so queue up and give the CPU away.
  res->waiters[res->waiter_count++] = task;
  task->state = TASK_BLOCKED;
  task->waiting_on = id;
  resource_trace(task, "waits for", id);

  task_next();  // Does not return
  return 0;
}

// Releases the resource. Pass ownership to oldest waiter.
int resource_unlock(int id) {
  struct task* task = task_current();
  struct resource* res = resource_get(id);

  if (!res) {
    return -EINVARG;
  }
  if (res->owner != task) {
    return -ENOTOWNER;
  }

  resource_trace(task, "released", id);

  if (res->waiter_count == 0) {
    res->owner = 0;
    return 0;
  }

  struct task* next = res->waiters[0];
  for (int i = 1; i < res->waiter_count; i++) {
    res->waiters[i - 1] = res->waiters[i];
  }
  res->waiter_count--;

  res->owner = next;
  next->waiting_on = -1;
  next->state = TASK_READY;
  next->registers.eax = 0;  // Return value of its lock() call
  resource_trace(next, "got", id);

  return 0;
}
