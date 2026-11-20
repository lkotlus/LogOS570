[BITS 32]

section .asm

global _start

; Test locking (every copy repeatedly takes R0)
_start:
    mov eax, 3 ; getpid()
    int 0x80
    add al, '0'
    mov [message + 5], al ; patch the digit into "Task ?"

.loop:
    push dword 0 ; lock(R0)
    mov eax, 4
    int 0x80
    add esp, 4

    push message ; print(message)
    mov eax, 1
    int 0x80
    add esp, 4

    mov ecx, 0x02000000 ; busy-wait while holding R0
.spin:
    dec ecx
    jnz .spin

    push dword 0 ; unlock(R0)
    mov eax, 5
    int 0x80
    add esp, 4

    push dword 20 ; sleep for 20 ticks
    mov eax, 2
    int 0x80
    add esp, 4

    jmp .loop

section .data
message: db 'Task ? in critical section', 10, 0
