#ifndef ISR80H_MISC_H
#define ISR80H_MISC_H

struct interrupt_frame;
void* isr80h_command0_sum(struct interrupt_frame* frame);
void* isr80h_command2_sleep(struct interrupt_frame* frame);
void* isr80h_command3_getpid(struct interrupt_frame* frame);
void* isr80h_command4_acquire(struct interrupt_frame* frame);
void* isr80h_command5_release(struct interrupt_frame* frame);
void* isr80h_command6_exit(struct interrupt_frame* frame);

#endif
