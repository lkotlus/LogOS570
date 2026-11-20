#include "misc.h"

#include "idt/idt.h"
#include "task/process.h"
#include "task/task.h"

void* isr80h_command0_sum(struct interrupt_frame* frame) {
  int v2 = (int)task_get_stack_item(task_current(), 1);
  int v1 = (int)task_get_stack_item(task_current(), 0);
  return (void*)(v1 + v2);
}

// sleep(ticks): arg pushed on the user stack. sleep(0) yields.
void* isr80h_command2_sleep(struct interrupt_frame* frame) {
  uint32_t ticks = (uint32_t)task_get_stack_item(task_current(), 0);
  task_sleep_current(ticks);  // Switches tasks; does not return.
  return 0;
}

// getpid(): returns the process slot number.
void* isr80h_command3_getpid(struct interrupt_frame* frame) {
  return (void*)(uint32_t)task_current()->process->id;
}
