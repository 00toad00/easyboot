.section .data

.global     gdt
.global     gdt_descriptor

gdt:
gdt_null:
    .quad   0
gdt_code:
    .word   0xFFFF
    .word   0x0000
    .byte   0x00
    .byte   0x9A
    .byte   0xCF
    .byte   0x00
gdt_data:
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
    