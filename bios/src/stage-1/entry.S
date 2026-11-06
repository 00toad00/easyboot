.section .text
.global _start

.extern stage2_entry

// disk.S
.extern read
.extern read_stage2
.extern dap
.extern bootdisk

.code16

_start:
    //  Disable interrupts and reset CS=0x0000
    cli
    ljmp    $0x0000, $1f
1:
    //  Stack/Frame and segment setup
    xorw    %ax, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss
    movw    $0x7C00, %sp
    pushw   $0
    movw    %sp, %bp
    sti

    //  Save boot disk
    movb    %dl, bootdisk

    movw    $0x0003, %ax
    int     $0x10

    movw    $0xB800, %ax
    movw    %ax, %es
    movw    $0, %di

    movb    $'X', %al
    movb    $0x0F, %ah
    stosw

    //  Read stage 2 from disk
    movl    stage2_lba, %eax
    movw    stage2_size, %cx
    call    read_stage2

    //  Switch to PM
    cli
    //  Enable A20 gate
    inb     $0x92, %al
    orb     $0x02, %al
    andb    $0xFE, %al
    outb    %al, $0x92

    //  GDT + PE
    lgdt    gdt_ptr
    movl    %cr0, %eax
    orl     $1, %eax
    movl    %eax, %cr0
    ljmpl   $0x08, $pm_entry

halt16:
    cli
    hlt
    jmp     halt16

.code32

pm_entry:
    movl    $0xB8000, %eax
    movb    $'0', 0(%eax)
    movb    $0x0F, 1(%eax)

    call    halt32
    //  Jump to extended boot code
    jmp     stage2_entry

halt32:
    cli
    hlt
    jmp     halt32

.section .data

gdt:
    .quad   0
    .quad   0x00CF9A000000FFFF
    .quad   0x00CF92000000FFFF
gdt_end:
gdt_ptr:
    .word   gdt_end - gdt - 1
    .long   gdt

//  Filled by installer
.section .patch

stage2_lba:     .long
stage2_size:    .word
