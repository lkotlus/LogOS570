#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

#define LOGOS_TOTAL_INTERRUPTS 512

// 100MB heap size
#define LOGOS_HEAP_SIZE_BYTES 104857600
#define LOGOS_HEAP_BLOCK_SIZE 4096
#define LOGOS_HEAP_ADDRESS 0x01000000
#define LOGOS_HEAP_TABLE_ADDRESS 0x00007E00
#define LOGOS_SECTOR_SIZE 512
#define LOGOS_MAX_FILESYSTEMS 12
#define LOGOS_MAX_FILE_DESCRIPTORS 512

#define LOGOS_TOTAL_GDT_SEGMENTS 6

#define LOGOS_PROGRAM_VIRTUAL_ADDRESS 0x400000
#define LOGOS_USER_PROGRAM_STACK_SIZE 1024 * 16
#define LOGOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START 0x3FF000
#define LOGOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END \
  LOGOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START - LOGOS_USER_PROGRAM_STACK_SIZE

#define USER_DATA_SEGMENT 0x23
#define USER_CODE_SEGMENT 0x1b

#define LOGOS_MAX_PROGRAM_ALLOCATIONS 1024
#define LOGOS_MAX_PROCESSES 12

#define LOGOS_MAX_ISR80H_COMMANDS 1024

#define LOGOS_KEYBOARD_BUFFER_SIZE 1024

// Round-robin time slice at 10ms
#define LOGOS_TIMER_HZ 100

// How many lockable resources we have
#define LOGOS_MAX_RESOURCES 8

// Print every lock/unlock/block (1 = on, 0 = off)
#define LOGOS_TRACE_RESOURCES 1

// Which test scenario kernel_main loads (programs/<name>/<name>.c)
#define LOGOS_SCENARIO_CONTROL 0      // 3 tasks: never deadlocks
#define LOGOS_SCENARIO_ABBA 1         // 2 tasks: always deadlocks in the first cycle
#define LOGOS_SCENARIO_RACY 2         // 2 tasks: eventually deadlocks
#define LOGOS_SCENARIO_PHILO 3        // 5 dining philosophers: 5-task cycle to deadlock
#define LOGOS_SCENARIO_EXITLOCK 4     // 3 tasks: exit while holding locks to release them
#define LOGOS_SCENARIO_POOL 5         // 3 tasks: deadlocks with no second resource (3-unit pool)
#define LOGOS_SCENARIO_SAFECYCLE 6    // 3 tasks: wait-for cycle that isn't deadlock due to multiple resources

#define LOGOS_SCENARIO LOGOS_SCENARIO_POOL

#endif
