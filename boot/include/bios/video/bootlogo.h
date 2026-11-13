#pragma once

#include <stdint.h>
#include <video/framebuffer.h>

typedef struct {
    int size;
    int width;
    int height;
} __attribute__((packed)) bootlogo_hdr;

extern const uint8_t bootlogo[];
extern const uint8_t bootlogo_end[];

void bootlogo_draw(const framebuffer *fb, const void *logo, uint32_t fg, uint32_t bg);