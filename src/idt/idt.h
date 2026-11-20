#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define PIT_BASE_FREQUENCY 1193182
#define PIT_CHANNEL0_PORT 0x40
#define PIT_COMMAND_PORT 0x43
#define PIT_COMMAND_CH0_LOHI_MODE3 0x36

struct idt_desc {
  uint16_t offset_1;
  uint16_t selector;
  uint8_t zero;
  uint8_t type_attr;
  uint16_t offset_2;
} __attribute__((packed));

struct idtr_desc {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

struct interrupt_frame {
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t reserved;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  uint32_t ip;
  uint32_t cs;
  uint32_t flags;
  uint32_t esp;
  uint32_t ss;
} __attribute__((packed));

struct interrupt_frame;
typedef void* (*ISR80H_COMMAND)(struct interrupt_frame* frame);

void idt_init();
void enable_interrupts();
void disable_interrupts();
void isr80h_register_command(int command_id, ISR80H_COMMAND command);
void timer_init(uint32_t hz);

#endif
