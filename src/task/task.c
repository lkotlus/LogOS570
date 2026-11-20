#include "task.h"

#include "idt/idt.h"
#include "kernel.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "memory/paging/paging.h"
#include "process.h"
#include "status.h"
#include "string/string.h"

// The current task that is running
struct task* current_task = 0;
// Task linked list
struct task* task_tail = 0;
struct task* task_head = 0;

int task_init(struct task* task, struct process* process);
struct task* task_current() { return current_task; }

struct task* task_new(struct process* process) {
  int res = 0;
  struct task* task = kzalloc(sizeof(struct task));

  if (!task) {
    res = -ENOMEM;
    goto out;
  }

  res = task_init(task, process);

  if (res != LOGOS_ALL_OK) {
    goto out;
  }
  if (task_head == 0) {
    task_head = task;
    task_tail = task;
    current_task = task;
    goto out;
  }

  task_tail->next = task;
  task->prev = task_tail;
  task_tail = task;

out:
  if (ISERR(res)) {
    task_free(task);
    return ERROR(res);
  }
  return task;
}

struct task* task_get_next() {
  if (!current_task->next) {
    return task_head;
  }
  return current_task->next;
}

static void task_list_remove(struct task* task) {
  if (task == current_task) {
    struct task* next = task_get_next();
    current_task = (next == task) ? 0 : next;
  }
  if (task->prev) {
    task->prev->next = task->next;
  }
  if (task->next) {
    task->next->prev = task->prev;
  }
  if (task == task_head) {
    task_head = task->next;
  }
  if (task == task_tail) {
    task_tail = task->prev;
  }
  task->next = 0;
  task->prev = 0;
}

int task_free(struct task* task) {
  paging_free_4gb(task->page_directory);
  task_list_remove(task);
  kfree(task);
  return 0;
}

int task_init(struct task* task, struct process* process) {
  memset(task, 0, sizeof(struct task));
  task->page_directory =
      paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

  if (!task->page_directory) {
    return -EIO;
  }

  task->registers.ip = LOGOS_PROGRAM_VIRTUAL_ADDRESS;
  task->registers.ss = USER_DATA_SEGMENT;
  task->registers.cs = USER_CODE_SEGMENT;
  task->registers.esp = LOGOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START;
  task->process = process;
  task->state = TASK_READY;
  task->waiting_on = -1;

  return 0;
}

int task_switch(struct task* task) {
  current_task = task;
  paging_switch(task->page_directory);
  return 0;
}

void task_run_first_ever_task() {
  if (!current_task) {
    panic("task_run_first_ever_task(): No current task exists!\\n");
  }
  task_switch(task_head);
  task_head->state = TASK_RUNNING;
  task_return(&task_head->registers);
}

int task_page() {
  user_registers();
  task_switch(current_task);
  return 0;
}

void task_save_state(struct task* task, struct interrupt_frame* frame) {
  task->registers.ip = frame->ip;
  task->registers.cs = frame->cs;
  task->registers.flags = frame->flags;
  task->registers.esp = frame->esp;
  task->registers.ss = frame->ss;
  task->registers.eax = frame->eax;
  task->registers.ebp = frame->ebp;
  task->registers.ebx = frame->ebx;
  task->registers.ecx = frame->ecx;
  task->registers.edi = frame->edi;
  task->registers.edx = frame->edx;
  task->registers.esi = frame->esi;
}

void task_current_save_state(struct interrupt_frame* frame) {
  if (!task_current()) {
    panic("No current task to save\n");
  }
  struct task* task = task_current();
  task_save_state(task, frame);
}

int copy_string_from_task(struct task* task, void* virtual, void* phys,
                          int max) {
  if (max >= PAGING_PAGE_SIZE) {
    return -EINVARG;
  }

  int res = 0;
  char* tmp = kzalloc(max);

  if (!tmp) {
    res = -ENOMEM;
    goto out;
  }

  uint32_t* task_directory = task->page_directory->directory_entry;
  uint32_t old_entry = paging_get(task_directory, tmp);
  paging_map(task->page_directory, tmp, tmp,
             PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

  paging_switch(task->page_directory);
  strncpy(tmp, virtual, max);
  kernel_page();
  res = paging_set(task_directory, tmp, old_entry);

  if (res < 0) {
    res = -EIO;
    goto out_free;
  }

  strncpy(phys, tmp, max);

out_free:
  kfree(tmp);
out:
  return res;
}

int task_page_task(struct task* task) {
  user_registers();
  paging_switch(task->page_directory);
  return 0;
}

void* task_get_stack_item(struct task* task, int index) {
  void* result = 0;
  uint32_t* sp_ptr = (uint32_t*)task->registers.esp;
  task_page_task(task);
  result = (void*)sp_ptr[index];
  kernel_page();

  return result;
}

//
// Round-robin scheduler code:
//

// Timer ticks since the PIT was started. Only written by the timer interrupt.
static volatile uint32_t task_ticks = 0;

uint32_t task_get_ticks() { return task_ticks; }

static int task_is_runnable(struct task* task) {
  return task->state == TASK_READY || task->state == TASK_RUNNING;
}

// Walks the list one time to find the next runnable task
static struct task* task_find_next_runnable() {
  if (!task_head) {
    return 0;
  }

  struct task* start = current_task ? current_task : task_head;
  struct task* task = start;
  do {
    task = task->next ? task->next : task_head;
    if (task_is_runnable(task)) {
      return task;
    }
  } while (task != start);

  return 0;
}

// Called on every timer tick, in both user and idle context. Wakes sleepers.
void task_tick() {
  task_ticks++;

  for (struct task* task = task_head; task; task = task->next) {
    // Signed difference so this still works when task_ticks wraps around.
    if (task->state == TASK_SLEEPING &&
        (int32_t)(task_ticks - task->wake_tick) >= 0) {
      task->state = TASK_READY;
    }
  }
}

// Switch to the next runnable task without returning
void task_next() {
  struct task* next = task_find_next_runnable();

  while (!next) {
    task_idle_wait();  // sti; hlt; cli: sleep until the next interrupt
    next = task_find_next_runnable();
  }

  if (current_task && current_task->state == TASK_RUNNING) {
    current_task->state = TASK_READY;
  }
  next->state = TASK_RUNNING;

  task_switch(next);
  task_return(&next->registers);
}

// Puts the current task to sleep for a set number of ticks and runs soemthing else.
void task_sleep_current(uint32_t ticks) {
  struct task* task = task_current();

  task->registers.eax = 0;

  if (ticks == 0) {
    task->state = TASK_READY;
  } else {
    task->state = TASK_SLEEPING;
    task->wake_tick = task_ticks + ticks;
  }

  task_next();
}
