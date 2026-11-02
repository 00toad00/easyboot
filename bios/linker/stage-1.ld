ENTRY(_start);
OUTPUT_FORMAT(binary);

SECTIONS {
    . = 0x7C00;
    .text   : { *(.text); }
    .data   : { *(.data); }
    .rodata : { *(.rodata); }

    ASSERT(. <= 0x7C00 + 420, "Stage 1 code too large!")

    . = 0x7C00 + 420;
    .patch  : { *(.patch); }
    . = 0x7C00 + 440;
    .mbr    : { *(.mbr); }
    . = 0x7C00 + 510;
    .sig    : { SHORT(0xAA55) }

    .bss    : {
        __bss_start = .;
        *(.bss .bss.*);
        *(COMMON) 
        __bss_end = .;
    }

    . = 0x7C00 + 1024;
    .stage2 : {
        stage2_magic = .;
        . += 4;
        stage2_entry = .;
    }
    ASSERT(__bss_end <= stage2_magic, "Stage 1 .bss overlaps stage 2 load addr")

    /DISCARD/ : { *(.sig) *(.note*) *(.comment) *(.eh_frame*) }
}