ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
    jmp short start ; explained later
    nop
    times 33 db 0 ; BIOS Parameter Block

start:
    jmp 0:step2 ; reset the CS with a long jump

step2:
    cli ; disable interrupts

    ; The origin is 0x7c00, so we'll set other
    ; segments to 0x00 so as to not overlap
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    sti ; enable interrupts

.load_protected:
    cli ; disable interrupts (they don't get re-enabled unless necessary)
    lgdt[gdt_descriptor] ; load the gdt addr into GDTR

    ; setting cr0 to cr0 OR 0x1 (protected mode bit set to 1)
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp CODE_SEG:load32

; GDT
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

; offset 0x8 (the 8 null bytes defined above)
; CS should end up pointing to gdt_code
gdt_code:
    dw 0xffff ; Segment limit first 0-15 bits
    dw 0 ; Base first 0-15 bits
    db 0 ; Base 16-23 bits
    db 0x9a ; Access byte
    db 11001111b ; High 4 bit flags and the low 4 bit flags
    db 0 ; Base 24-31 bits

; offset 0x10 (the 16 bytes defined above, including 8 null)
gdt_data: ; DS, SS, ES, FS, GS
    dw 0xffff ; Segment limit first 0-15 bits
    dw 0 ; Base first 0-15 bits
    db 0 ; Base 16-23 bits
    db 0x92 ; Access byte
    db 11001111b ; High 4 bit flags and the low 4 bit flags
    db 0 ; Base 24-31 bits

; end of our GDT
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start-1 ; address arithmetic :)
    dd gdt_start

; start of protected mode!
[BITS 32]
load32:
    ; configure all our segments
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp
    
    ; Enabling the A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    mov eax, 1                 ; LDA (1 is the address of sector 2)
    mov ecx, 100               ; Read 100 sectors
    mov edi, 0x0100000         ; Read them to 0x0100000 in memory
    call ata_lba_read          ; Call the read routine
    jmp CODE_SEG:0x0100000     ; Jump to our kernel at 0x0100000 in the code segment

    jmp $ ; infinite loop

ata_lba_read:
    mov ebx, eax ; backup the LBA
    
    ; send the highest 8 bits of the LBA to the HDD controller
    shr eax, 24
    or eax, 0xe0
    mov dx, 0x1f6
    out dx, al
    
    ; Send the total sectors to read
    mov eax, ecx
    mov dx, 0x1f2
    out dx, al
    
    ; Send more bits of the LBA
    mov eax, ebx ; restore the LBA backup
    mov dx, 0x1F3
    out dx, al
    
    ; Send more bits of the LBA
    mov dx, 0x1f4
    mov eax, ebx ; restore the LBA backup
    shr eax, 8
    out dx, al
    shr eax, 8
    
    ; Send upper 16 bits of the LBA
    mov dx, 0x1f5
    mov eax, ebx ; restore the LBA backup
    shr eax, 16
    out dx, al
    
    mov dx, 0x1f7
    mov al, 0x20
    out dx, al
    
    .next_sector:
        push ecx
        
        .try_again:
            mov dx, 0x1f7
            in al, dx
            test al, 8
            jz .try_again
    
        mov ecx, 256
        mov dx, 0x1f0
        rep insw
        pop ecx
        loop .next_sector
        
    ret

; padding and signature
times 510-($ - $$) db 0
dw 0xAA55
