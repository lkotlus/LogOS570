#ifndef ISR80H_H
#define ISR80H_H

enum SystemCommands {
  SYSTEM_COMMAND0_SUM,
  SYSTEM_COMMAND1_PRINT,
  SYSTEM_COMMAND2_SLEEP,
  SYSTEM_COMMAND3_GETPID,
};

void isr80h_register_commands();

#endif
