[BITS 32]

; Linked first so _start is the byte the kernel jumps to.
section .start

global _start
extern main

_start:
    call main

    ; No exit syscall yet (sleep forever)
.done:
    push dword 1000
    mov eax, 2 ; sleep
    int 0x80
    add esp, 4
    jmp .done
