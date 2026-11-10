.section .text.entry

.global _start
.extern main

.code32

_start:
    pushl   %ebp
    movl    %esp, %ebp

    call    main

    movl    %ebp, %esp
    popl    %ebp
    ret
    