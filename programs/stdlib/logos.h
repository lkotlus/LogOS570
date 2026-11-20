#ifndef LOGOS_H
#define LOGOS_H

#include <stdarg.h>
#include <stddef.h>

// Syscalls (stdlib/syscalls.asm)
void print(const char* str);
int sleep(unsigned int ticks);  // 100 ticks = 1 second, sleep(0) yields
int getpid();
int lock(int id);    // Blocks until the resource is ours. 0 or -error.
int unlock(int id);  // 0 or -error

// Library (stdlib/logos.c)
void printf(const char* fmt, ...);  // %d %u %x %s %c %%
void spin(unsigned int iterations);  // Busy-wait without giving up the CPU
void* memset(void* ptr, int c, size_t size);
void* memcpy(void* dest, const void* src, size_t len);

#endif
