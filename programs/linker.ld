ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 0x400000;

    /* .start (from start.asm) must come first: the kernel jumps to 0x400000. */
    .text : ALIGN(4096)
    {
        *(.start)
        *(.text*)
    }

    .rodata : ALIGN(4096)
    {
        *(.rodata*)
    }

    .data : ALIGN(4096)
    {
        *(.data*)
        *(COMMON)
        *(.bss*)
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame*)
        *(.note*)
    }
}
