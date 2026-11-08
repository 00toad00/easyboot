.section .text

.global get_controller_info

.macro SET_SEGMENT value
    movw    \value, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss
.endm

.macro SAVE_REGS
    pushl   %eax
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    pushl   %esi
    pushl   %edi
    pushl   %ebp
.endm

.macro LOAD_REGS
    popl    %ebp
    popl    %edi
    popl    %esi
    popl    %edx
    popl    %ecx
    popl    %ebx
    popl    %eax
.endm

.code32
get_controller_info:
    pushl   %ebp
    movl    %esp, %ebp

    movl    8(%ebp), %eax
    movl    %eax, get_controller_info_ptr

    SAVE_REGS
    movl    %esp, esp_save

    cli
    ljmp    $0x18, $1f
.code16
1:
    SET_SEGMENT $0x20

    movl    %cr0, %eax
    andl    $~0x80000001, %eax
    movl    %eax, %cr0
    ljmp    $0x0000, $2f
2:
    SET_SEGMENT $0x00
    movw    $0x7C00, %sp
    sti

    movl    get_controller_info_ptr, %eax
    movw    %ax, %di
    andw    $0x000F, %di
    shrl    $4, %eax
    movw    %ax, %es

    movw    $0x4F00, %ax
    int     $0x10

    movw    %ax, get_controller_info_return

    cli
    movl    %cr0, %eax
    orl     $1, %eax
    movl    %eax, %cr0
    ljmpl   $0x08, $3f
.code32
3:
    SET_SEGMENT $0x10
    movl    esp_save, %esp
    LOAD_REGS

    xorl    %eax, %eax
    movw    get_controller_info_return, %ax

    movl    %ebp, %esp
    popl    %ebp
    ret

.section .data

esp_save:   .long 0

get_controller_info_return: .word 0
get_controller_info_ptr:    .long 0
