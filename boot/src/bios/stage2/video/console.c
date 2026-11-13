#include <stdarg.h>

#include <string.h>
#include <stdlib.h>

#include <video/console.h>
#include <video/font.h>

void draw_char(char c, int x, int y, framebuffer *fb) {
    unsigned char ch = (unsigned char)c;
    if (ch < FONT_FIRST || ch > FONT_LAST) ch = '?';

    const unsigned char *glyph = font[ch - FONT_FIRST];
    uint32_t bytes_pp = (fb->bpp + 7) / 8;

    for (int row = 0; row < FONT_H; row++) {
        uint8_t bits = glyph[FONT_H - 1 - row];
        uint8_t *line = (uint8_t *)fb->lfb + (y + row) * fb->pitch
                      + x * bytes_pp;

        for (int col = 0; col < FONT_W; col++) {
            if (!(bits & (0x80 >> col)))     
                continue;                
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

void print_c(const char c, console *con) {
    if (c == '\n') {
        con->pos.x = 0;
        con->pos.y += con->glyph.y + con->pad.y;
        return;
    }
    if (con->border.x + con->pos.x + con->glyph.x + con->pad.x
            > (int)(con->fb->width - con->border.x)) {
        con->pos.x = 0;
        con->pos.y += con->glyph.y + con->pad.y;
    }
    draw_char(c, con->border.x + con->pos.x, con->border.y + con->pos.y, con->fb);
    con->pos.x += con->glyph.x + con->pad.x;
}

void print_s(const char *str, console *con) {
    while (*str) print_c(*str++, con);
}

void printf(const char *fmt, console *con, ...) {
    va_list ap;
    va_start(ap, con);
    char buf[66];

    for (; *fmt; fmt++) {
        if (*fmt != '%') { print_c(*fmt, con); continue; }

        switch (*++fmt) {
        case 's': {
            const char *s = va_arg(ap, const char *);
            print_s(s ? s : "(null)", con);
            break;
        }
        case 'u': utoa(va_arg(ap, uint32_t), 10, buf);  print_s(buf, con); break;
        case 'x': utoa(va_arg(ap, uint32_t), 16, buf);  print_s(buf, con); break;
        case 'o': utoa(va_arg(ap, uint32_t), 8,  buf);  print_s(buf, con); break;
        case 'i': itoa(va_arg(ap, int32_t), buf);       print_s(buf, con); break;
        case '\0': fmt--; break;
        default:  break;
        }
    }
    va_end(ap);
}