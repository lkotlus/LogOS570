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

// Must be called before any task uses the resource. Resets it to all free.
void resource_set_capacity(int id, int capacity) {
  struct resource* res = resource_get(id);
  if (!res) {
    return;
  }
  res->capacity = capacity;
  res->available = capacity;
  res->waiter_count = 0;
}

// Prints e.g. "[T1 got n R0, (N-n)/N free]"
static void resource_trace(struct task* task, const char* what, int id,
                           int count) {
#if LOGOS_TRACE_RESOURCES
  struct resource* res = &resources[id];
  print("    [T");
  print_int(task->process->id);
  print(" ");
  print(what);
  print(" ");
  print_int(count);
  print("x R");
  print_int(id);
  print(", ");
  print_int(res->available);
  print("/");
  print_int(res->capacity);
  print(" free]\n");
#endif
}

static void resource_grant(struct resource* res, int id, struct task* task,
                           int count) {
  res->available -= count;
  task->held[id] += count;
  resource_trace(task, "got", id, count);
}

static void resource_dequeue(struct resource* res, int index) {
  for (int i = index + 1; i < res->waiter_count; i++) {
    res->waiters[i - 1] = res->waiters[i];
  }
  res->waiter_count--;
}

static void resource_grant_waiters(struct resource* res, int id) {
  while (res->waiter_count > 0 &&
         res->waiters[0]->waiting_count <= res->available) {
    struct task* next = res->waiters[0];
    resource_dequeue(res, 0);

    resource_grant(res, id, next, next->waiting_count);
    next->waiting_on = -1;
    next->waiting_count = 0;
    next->state = TASK_READY;
    next->registers.eax = 0;  // Return value of its acquire() call
  }
}

int resource_acquire(int id, int count) {
  struct task* task = task_current();
  struct resource* res = resource_get(id);

  if (!res || count < 1 || count > res->capacity) {
    return -EINVARG;
  }
  if (task->held[id] + count > res->capacity) {
    return -EISTKN;  // Could never be satisfied (e.g. relocking a mutex)
  }

  if (res->waiter_count == 0 && count <= res->available) {
    resource_grant(res, id, task, count);
    return 0;
  }

  res->waiters[res->waiter_count++] = task;
  task->state = TASK_BLOCKED;
  task->waiting_on = id;
  task->waiting_count = count;
  resource_trace(task, "waits for", id, count);

  task_next();  // Does not return
  return 0;
}

static void resource_return_units(struct task* task, int id, int count) {
  struct resource* res = &resources[id];
  task->held[id] -= count;
  res->available += count;
  resource_trace(task, "released", id, count);
  resource_grant_waiters(res, id);
}

int resource_release(int id, int count) {
  struct task* task = task_current();
  struct resource* res = resource_get(id);

  if (!res || count < 1) {
    return -EINVARG;
  }
  if (task->held[id] < count) {
    return -ENOTOWNER;
  }

  resource_return_units(task, id, count);
  return 0;
}

void resource_release_all(struct task* task) {
  for (int id = 0; id < LOGOS_MAX_RESOURCES; id++) {
    if (task->held[id] > 0) {
      resource_return_units(task, id, task->held[id]);
    }
  }
}

void resource_cancel_wait(struct task* task) {
  struct resource* res = resource_get(task->waiting_on);
  if (!res) {
    return;
  }

  for (int i = 0; i < res->waiter_count; i++) {
    if (res->waiters[i] == task) {
      resource_dequeue(res, i);
      break;
    }
  }

  int id = task->waiting_on;
  task->waiting_on = -1;
  task->waiting_count = 0;
  resource_grant_waiters(res, id);
}
