.section .text

.global bios_call

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

//  Offsets into struct regs (see bottom of file)
.set R_EAX,     0
.set R_EBX,     4
.set R_ECX,     8
.set R_EDX,     12
.set R_ESI,     16
.set R_EDI,     20
.set R_EBP,     24
.set R_DS,      28
.set R_ES,      30
.set R_EFLAGS,  32
.set R_SIZE,    36

.code32
//  extern "C" void bios_call(uint8_t intn, regs* r);
//
//  The caller's struct may live anywhere in 32-bit memory. It is copied into
//  a bounce buffer below 64 KiB, because real mode can only address it with
//  DS=0 and a 16-bit offset (segment limit is 0xFFFF).
bios_call:
    pushl   %ebp
    movl    %esp, %ebp

    SAVE_REGS

    //  Patch interrupt number into the int instruction
    movl    8(%ebp), %eax
    movb    %al, 5f

    //  Copy caller's regs -> bounce buffer
    cld
    movl    12(%ebp), %esi
    movl    $regs_buf, %edi
    movl    $(R_SIZE / 4), %ecx
    rep movsl

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

    //  Load input registers from bounce buffer (DS=0 here)
    movl    regs_buf + R_EAX, %eax
    movl    regs_buf + R_EBX, %ebx
    movl    regs_buf + R_ECX, %ecx
    movl    regs_buf + R_EDX, %edx
    movl    regs_buf + R_ESI, %esi
    movl    regs_buf + R_EDI, %edi
    movl    regs_buf + R_EBP, %ebp
    //  ES before DS: loading DS must be the last memory access
    pushw   regs_buf + R_ES
    popw    %es
    pushw   regs_buf + R_DS
    popw    %ds

4:
    .byte 0xCD
5:
    .byte 0

    //  Save flags and segments before touching memory, then restore DS=0
    pushfl
    pushw   %ds
    pushw   %es
    pushl   %eax
    xorw    %ax, %ax
    movw    %ax, %ds

    popl    %eax
    movl    %eax, regs_buf + R_EAX
    movl    %ebx, regs_buf + R_EBX
    movl    %ecx, regs_buf + R_ECX
    movl    %edx, regs_buf + R_EDX
    movl    %esi, regs_buf + R_ESI
    movl    %edi, regs_buf + R_EDI
    movl    %ebp, regs_buf + R_EBP
    popw    regs_buf + R_ES
    popw    regs_buf + R_DS
    popl    regs_buf + R_EFLAGS

    cli
    movl    %cr0, %eax
    orl     $1, %eax
    movl    %eax, %cr0
    ljmpl   $0x08, $3f
.code32
3:
    SET_SEGMENT $0x10
    movl    esp_save, %esp

    //  Copy bounce buffer -> caller's regs
    //  (%ebp was clobbered in real mode; the frame pointer is the last
    //  value SAVE_REGS pushed, so it sits at (%esp))
    cld
    movl    (%esp), %edi
    movl    12(%edi), %edi
    movl    $regs_buf, %esi
    movl    $(R_SIZE / 4), %ecx
    rep movsl

    LOAD_REGS

    movl    %ebp, %esp
    popl    %ebp
    ret

.section .data

.balign 4
esp_save:   .long 0
regs_buf:   .skip R_SIZE

/*
struct regs {
    uint32_t eax;       //  0
    uint32_t ebx;       //  4
    uint32_t ecx;       //  8
    uint32_t edx;       // 12
    uint32_t esi;       // 16
    uint32_t edi;       // 20
    uint32_t ebp;       // 24
    uint16_t ds;        // 28
    uint16_t es;        // 30
    uint32_t eflags;    // 32  (output only)
} __attribute__((packed));
*/
