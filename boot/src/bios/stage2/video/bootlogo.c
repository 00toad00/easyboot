#include <video/bootlogo.h>

void bootlogo_draw(const framebuffer *fb, const void *logo, uint32_t fg, uint32_t bg) {
    const bootlogo_hdr *hdr = (const bootlogo_hdr *)logo;
    const uint8_t *bits = (const uint8_t *)(hdr + 1);
    uint32_t w = (uint32_t)hdr->width, h = (uint32_t)hdr->height;
    uint32_t bytes = fb->bpp / 8;                                   /* bytes per pixel: 4 or 3 */

    if (bytes != 4 && bytes != 3) return;
    if (hdr->width <= 0 || hdr->height <= 0) return;
    if ((uint64_t)hdr->size * 8 < (uint64_t)w * h) return;          /* header does not match the data */
    if (w > fb->width || h > fb->height) return;                    /* does not fit on this screen */

    uint32_t ox = (fb->width - w) / 2;                              /* left edge: centered horizontally */
    uint32_t oy = (fb->height - h) / 2;                             /* top edge: centered vertically */

    for (uint32_t y = 0; y < h; y++) {
        uint8_t *p = (uint8_t *)fb->lfb + (uint64_t)(oy + y) * fb->pitch + (uint64_t)ox * bytes;
        for (uint32_t x = 0; x < w; x++, p += bytes) {
            uint32_t i = y * w + x;                                 /* bit number in the stream */
            uint32_t c = ((bits[i >> 3] >> (7 - (i & 7))) & 1) ? fg : bg;
            p[0] = (uint8_t)c;                                      /* blue */
            p[1] = (uint8_t)(c >> 8);                               /* green */
            p[2] = (uint8_t)(c >> 16);                              /* red */
            if (bytes == 4) p[3] = 0;
        }
    }
}

__asm__(
    ".pushsection .rodata\n"
    ".global bootlogo\n"
    ".balign 4\n"
    "bootlogo:\n"
    ".incbin \"video/bootlogo.bin\"\n"
    ".global bootlogo_end\n"
    "bootlogo_end:\n"
    ".popsection\n"  
);