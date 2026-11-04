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
    ljmp    $0x0000, $boot16
boot16:
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

    //  Read stage 2 from disk
    movl    stage2_lba, %eax
    movw    stage2_size, %cx
    call    read_stage2

    call halt
    //  Jump to extended boot code
    jmp     stage2_entry

halt:
    cli
    hlt
    jmp     halt

//  Filled by installer
.section .patch

stage2_lba:     .long
stage2_size:    .word
