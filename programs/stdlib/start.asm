[BITS 32]

; Linked first so _start is the byte the kernel jumps to.
section .start

global _start
extern main

_start:
    call main

    push eax
    mov eax, 6
    int 0x80
