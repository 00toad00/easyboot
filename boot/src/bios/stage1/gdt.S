.section .data

.global     gdt
.global     gdt_descriptor

gdt:
gdt_null:                   // 0x00
    .quad   0
gdt_code32:                 // 0x08
    .word   0xFFFF
    .word   0x0000
    .byte   0x00
    .byte   0x9A
    .byte   0xCF
    .byte   0x00
gdt_data32:                 // 0x10
    .word   0xFFFF
    .word   0x0000
    .byte   0x00
    .byte   0x92
    .byte   0xCF
    .byte   0x00
gdt_code16:                 // 0x18
    .word   0xFFFF
    .word   0x0000
    .byte   0x00
    .byte   0x9A
    .byte   0x00
    .byte   0x00
gdt_data16:                 // 0x20
    .word   0xFFFF
    .word   0x0000
    .byte   0x00
    .byte   0x92
    .byte   0x00
    .byte   0x00
gdt_code64:                 // 0x28
    .word   0xFFFF
    .word   0x0000
    .byte   0x00
    .byte   0x9A
    .byte   0xAF
    .byte   0x00
gdt_data64:                 // 0x30
    .word   0xFFFF
    .word   0x0000
    .byte   0x00
    .byte   0x92
    .byte   0xCF
    .byte   0x00
gdt_end:
gdt_descriptor:
    .word   gdt_end - gdt - 1
    .long   gdt
