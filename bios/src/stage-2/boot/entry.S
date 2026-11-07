.section .text

.global _start

.code32

_start:
    push    %ebp
    movl    %esp, %ebp

    movl    $0xB8000, %eax
    movb    $'1', 0(%eax)
    movb    $0x0F, 1(%eax) 

    movl    %ebp, %esp
    popl    %ebp
    ret
