#include "logos.h"

#define PRINTF_BUFFER_SIZE 256

struct out {
  char buf[PRINTF_BUFFER_SIZE];
  int len;
};

static void out_char(struct out* o, char c) {
  if (o->len < PRINTF_BUFFER_SIZE - 1) {
    o->buf[o->len++] = c;
  }
}

static void out_str(struct out* o, const char* s) {
  while (*s) {
    out_char(o, *s++);
  }
}

static void out_uint(struct out* o, unsigned int n, unsigned int base) {
  char digits[11];
  int i = 0;
  do {
    unsigned int d = n % base;
    digits[i++] = d < 10 ? '0' + d : 'a' + (d - 10);
    n /= base;
  } while (n);
  while (i > 0) {
    out_char(o, digits[--i]);
  }
}

// Formats into a buffer, prints with one syscall so another taks
// doesn't get a CS and split our output
void printf(const char* fmt, ...) {
  struct out o;
  o.len = 0;

  va_list args;
  va_start(args, fmt);
  for (; *fmt; fmt++) {
    if (*fmt != '%') {
      out_char(&o, *fmt);
      continue;
    }

    fmt++;
    switch (*fmt) {
      case 'd': {
        int v = va_arg(args, int);
        if (v < 0) {
          out_char(&o, '-');
          out_uint(&o, -(unsigned int)v, 10);
        } else {
          out_uint(&o, v, 10);
        }
        break;
      }
      case 'u':
        out_uint(&o, va_arg(args, unsigned int), 10);
        break;
      case 'x':
        out_uint(&o, va_arg(args, unsigned int), 16);
        break;
      case 's':
        out_str(&o, va_arg(args, const char*));
        break;
      case 'c':
        out_char(&o, (char)va_arg(args, int));
        break;
      case '%':
        out_char(&o, '%');
        break;
      case 0:
        fmt--;
        break;
      default:
        out_char(&o, '%');
        out_char(&o, *fmt);
        break;
    }
  }
  va_end(args);

  o.buf[o.len] = 0;
  print(o.buf);
}

void spin(unsigned int iterations) {
  for (volatile unsigned int i = 0; i < iterations; i++) {
  }
}

void* memset(void* ptr, int c, size_t size) {
  char* p = ptr;
  for (size_t i = 0; i < size; i++) {
    p[i] = (char)c;
  }
  return ptr;
}

void* memcpy(void* dest, const void* src, size_t len) {
  char* d = dest;
  const char* s = src;
  for (size_t i = 0; i < len; i++) {
    d[i] = s[i];
  }
  return dest;
}
