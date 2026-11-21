#ifndef LOGOS_H
#define LOGOS_H

#include <stdarg.h>
#include <stddef.h>

// Syscalls (stdlib/syscalls.asm)
void print(const char* str);
int sleep(unsigned int ticks);  // 100 ticks = 1 second, sleep(0) yields
int getpid();
int acquire(int id, int count);  // Blocks until all required units are ours
int release(int id, int count);  // 0 or -error

// Mutex shorthand
static inline int lock(int id) { return acquire(id, 1); }
static inline int unlock(int id) { return release(id, 1); }

// Library (stdlib/logos.c)
void printf(const char* fmt, ...);  // %d %u %x %s %c %%
void spin(unsigned int iterations);  // Busy-wait without giving up the CPU
void* memset(void* ptr, int c, size_t size);
void* memcpy(void* dest, const void* src, size_t len);
void exit(int status) __attribute__((noreturn)); // Releases all locks

#endif
