#pragma once

#include <stdint.h>

typedef struct {
    void        *lfb;
    uint32_t    width;
    uint32_t    height;
    uint32_t    pitch;
    uint8_t     bpp;
} framebuffer;