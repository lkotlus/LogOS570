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

int process_load_for_slot(const char* filename, struct process** process,
                          int process_slot);
int process_load(const char* filename, struct process** process);

#endif
