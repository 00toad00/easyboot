.section .text

.global read
.global read_stage2

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

read_stage2:
    movl    %eax, dap.lba
    movl    $0, dap.lba+4
    movw    $0, dap.off
    movw    $0x0800, dap.seg
1:
    jcxz    3f
    movw    %cx, %bx
    cmpw    $64, %bx
    jbe     2f
    movw    $64, %bx
2:
    movw    %bx, dap.n
    pushw   %bx
    pushw   %cx
    pushw   $dap
    call    read
    addw    $2, %sp
    popw    %cx
    popw    %bx
    jc      4f

    subw    %bx, %cx
    movzwl  %bx, %eax
    addl    %eax, dap.lba
    shlw    $5, %bx
    addw    %bx, dap.seg
    jmp     1b
3:
    clc
4: 
    ret

.section .data

.global bootdisk
.global dap

bootdisk:   .byte 0

dap:
dap.size:   .byte 16
dap.zero:   .byte 0
dap.n:      .word 0
dap.off:    .word 0
dap.seg:    .word 0
dap.lba:    .quad 0
    