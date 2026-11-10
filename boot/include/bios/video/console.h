#pragma once

#include <stdint.h>
#include <stdarg.h>

typedef struct {
    char        *vga;
    uint32_t    width;
    uint32_t    height;
    uint32_t    i;
} console;

void putc(console *con, char c);
void print(console *con, const char *str);
void clear(console *con);