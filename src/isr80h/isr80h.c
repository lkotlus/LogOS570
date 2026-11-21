#include "isr80h.h"

#include "idt/idt.h"
#include "io.h"
#include "misc.h"

void isr80h_register_commands() {
  isr80h_register_command(SYSTEM_COMMAND0_SUM, isr80h_command0_sum);
  isr80h_register_command(SYSTEM_COMMAND1_PRINT, isr80h_command1_print);
  isr80h_register_command(SYSTEM_COMMAND2_SLEEP, isr80h_command2_sleep);
  isr80h_register_command(SYSTEM_COMMAND3_GETPID, isr80h_command3_getpid);
  isr80h_register_command(SYSTEM_COMMAND4_ACQUIRE, isr80h_command4_acquire);
  isr80h_register_command(SYSTEM_COMMAND5_RELEASE, isr80h_command5_release);
  isr80h_register_command(SYSTEM_COMMAND6_EXIT, isr80h_command6_exit);
}
