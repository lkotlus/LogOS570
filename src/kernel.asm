[BITS 32]

global _start
extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp
    
    ; Enable A20
    in al, 0x92
    or al, 2
    out 0x92, al
    
    ; Send signal to master PIC
    mov al, 00010001b
    out 0x20, al

    ; Set the start interrupt to 0x20
    mov al, 0x20
    out 0x21, al

    ; Remap
    mov al, 00000001b
    out 0x21, al

    ; Enable interrupts
    sti

    call kernel_main

    jmp $

; Pad with null to fill up to the next 512 bytes
times 512-($ - $$) db 0
