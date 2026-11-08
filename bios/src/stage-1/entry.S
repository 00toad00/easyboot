.section .text
.global _start

.extern stage2_entry

// disk.S
.extern read
.extern read_stage2
.extern dap
.extern bootdisk

// gdt.S
.extern gdt_descriptor

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

    //  Video Mode set to 0x03
    movw    $0x0003, %ax
    int     $0x10

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
    lgdt    gdt_descriptor
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
    //  Segments with 32bit data descriptor
    movw    $0x10, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss
    movl    $0x90000, %esp

    pushl   %ebp
    movl    %esp, %ebp

    //  Jump to extended boot code
    xorl    %eax, %eax
    movb    bootdisk, %al
    pushl   %eax
    call    stage2_entry
    call    halt32

halt32:
    cli
    hlt
    jmp     halt32

//  Filled by installer
.section .patch, "a"

stage2_lba:     .long 2048
stage2_size:    .word 1
