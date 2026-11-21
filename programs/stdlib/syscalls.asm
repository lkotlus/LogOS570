[BITS 32]

section .text

%macro SYSCALL0 2
global $%1
$%1:
    mov eax, %2
    int 0x80
    ret
%endmacro

%macro SYSCALL1 2
global $%1
$%1:
    push ebp
    mov ebp, esp
    push dword [ebp+8]
    mov eax, %2
    int 0x80
    add esp, 4
    pop ebp
    ret
%endmacro

%macro SYSCALL2 2
global $%1
$%1:
    push ebp
    mov ebp, esp
    push dword [ebp+12]
    push dword [ebp+8]
    mov eax, %2
    int 0x80
    add esp, 8
    pop ebp
    ret
%endmacro

SYSCALL1 print,     1
SYSCALL1 sleep,     2
SYSCALL0 getpid,    3
SYSCALL1 acquire,   4
SYSCALL1 release,   5
SYSCALL1 exit,      6
