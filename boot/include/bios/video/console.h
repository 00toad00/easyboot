#pragma once

#include <video/framebuffer.h>

typedef struct {
    int x;
    int y;
} dim2;

typedef struct {
    framebuffer *fb;

    dim2 pos;
    dim2 border;

    dim2 glyph;
    dim2 pad;
} console;

void printf(const char *fmt, console *con, ...);