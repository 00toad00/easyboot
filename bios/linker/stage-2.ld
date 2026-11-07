ENTRY(_start)
OUTPUT_FORMAT(binary)

SECTIONS {
    . = 0x8000;
    .magic  : { LONG(0xB00710AD) }
    .text   : { *(.text); }
    .data   : { *(.data); }
    .rodata : { *(.rodata); }

    .bss    : {
        __bss_start = .;
        *(.bss .bss.*);
        *(COMMON)
        __bss_end = .;
    }    

    /DISCARD/ : { *(.note*) *(.comment) *(.eh_frame*) }
}