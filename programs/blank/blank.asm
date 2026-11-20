[BITS 32]

section .asm

global _start

_start:
    mov eax, 3 ; getpid()
    int 0x80
    add al, '0'
    mov [message + 5], al ; patch the digit into "Task ?"

.loop:
    push message
    mov eax, 1 ; print(message)
    int 0x80
    add esp, 4

    mov ecx, 0x02000000 ; busy-wait
.spin:
    dec ecx
    jnz .spin

    push dword 50 ; sleep for 50 ticks = 0.5s at 100Hz
    mov eax, 2
    int 0x80
    add esp, 4

    jmp .loop

section .data
message: db 'Task ? is running', 10, 0
