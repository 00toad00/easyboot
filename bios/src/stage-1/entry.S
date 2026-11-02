.section .text
.global _start

.extern stage2_entry

// disk.S
.extern read
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

    //  Save boot disk
    movb    %dl, bootdisk

    movw    $0x0003, %ax
    int     $0x10

    movw    $0xB800, %ax
    movw    %ax, %es
    xorw    %di, %di

    movb    $'X', %al
    movb    $0x0F, %ah
    stosw

    call halt
    /*TODO*/

    //  Jump to extended boot code
    jmp     stage2_entry

halt:
    cli
    hlt
    jmp     halt

//  Filled by installer
.section .patch

stage2_lba:     .long
stage2_size:    .long
