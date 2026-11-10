#pragma once

#include <stdint.h>

typedef struct {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint16_t ds;
    uint16_t es;
    uint32_t eflags;
} __attribute__((packed)) regs;

void bios_call(uint8_t intn, regs* r);