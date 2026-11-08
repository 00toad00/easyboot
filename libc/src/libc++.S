.section .text

.global __cxa_pure_virtual

__cxa_pure_virtual:
    cli
1:
    hlt
    jmp     1b
    