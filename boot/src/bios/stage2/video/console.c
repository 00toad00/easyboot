#include <video/console.h>
#include <video/font.h>

void draw_char(char c, int x, int y, framebuffer *fb) {
    unsigned char ch = (unsigned char)c;
    if (ch < FONT_FIRST || ch > FONT_LAST) ch = '?';

    const unsigned char *glyph = font[ch - FONT_FIRST];
    uint32_t bytes_pp = (fb->bpp + 7) / 8;

    for (int row = 0; row < FONT_H; row++) {
        // glyph[0] is the bottom row, so flip vertically
        uint8_t bits = glyph[FONT_H - 1 - row];
        uint8_t *line = (uint8_t *)fb->lfb + (y + row) * fb->pitch
                      + x * bytes_pp;

        for (int col = 0; col < FONT_W; col++) {
            if (!(bits & (0x80 >> col)))      // MSB = leftmost pixel
                continue;                     // transparent background
            uint8_t *p = line + col * bytes_pp;
            switch (fb->bpp) {
            case 32: *(uint32_t *)p = 0xFFFFFFFF; break;
            case 24: p[0] = p[1] = p[2] = 0xFF; break;
            case 16:
            case 15: *(uint16_t *)p = 0xFFFF; break;
            }
        }
    }
}