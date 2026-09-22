section .asm

global idt_load
idt_load:
    ; Set up our stack frame
    push ebp
    mov ebp, esp
    
    mov ebp, [ebp+8] ; Load the first argument into the ebx register
    lidt [ebx] ; Load the IDT into the CPU
    pop ebp ; Clean up the stack frame
    ret ; Return from the function
