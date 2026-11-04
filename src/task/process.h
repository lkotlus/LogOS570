#ifndef PROCESS_H
#define PROCESS_H
#include <stdint.h>

#include "config.h"
#include "kernel.h"
#include "task.h"

struct process {
  uint16_t id;
  char filename[LOGOS_MAX_PATH];
  struct task* task;
  void* allocations[LOGOS_MAX_PROGRAM_ALLOCATIONS];
  void* ptr;
  void* stack;
  uint32_t size;
};
#endif
