.section .text

.global read

.code16

//  CF clear = success
read:
    push    %bp
    movw    %sp, %bp

    movb    $0x42, %ah
    movb    bootdisk, %dl
    movw    4(%bp), %si
    int     $0x13

    movw    %bp, %sp
    popw    %bp
    ret

.section .bss

.global bootdisk
.global dap

bootdisk:  .skip 1

dap:
    .skip 1
    .skip 1
    .skip 2
    .skip 2
    .skip 2
    .skip 8
    