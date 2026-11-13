#include <stdarg.h>

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
    if (con->border.x+con->pos.x+con->glyph.x+con->pad.x > (int)(con->fb->width - con->border.x)) {
        con->pos.x = 0;
        con->pos.y += (con->glyph.y + con->pad.y);
    }
    draw_char(c, con->border.x+con->pos.x, con->border.y+con->pos.y, con->fb);
    con->pos.x += (con->glyph.x + con->pad.x);    
}

void print_s(const char *str, console *con) {
    for (int i = 0; str[i] != 0; i++) {
        if ((con->border.x+con->pos.x+con->glyph.x+con->pad.x > (int)(con->fb->width - con->border.x)) || str[i] == '\n') {
            con->pos.x = 0;
            con->pos.y += (con->glyph.y + con->pad.y);
            continue;
        }
        draw_char(str[i], con->border.x+con->pos.x, con->border.y+con->pos.y, con->fb);
        con->pos.x += (con->glyph.x + con->pad.x);
    }
}

void printf(const char *fmt, console *con, ...) {
    va_list ap;
    va_start(ap, con);

    for (; *fmt; fmt++) {
        if ((con->border.x+con->pos.x+con->glyph.x+con->pad.x > (int)(con->fb->width - con->border.x)) || *fmt == '\n') {
            con->pos.x = 0;
            con->pos.y += (con->glyph.y + con->pad.y);
            continue;
        } else if (*fmt != '%') { 
            print_c(*fmt, con); 
            continue; 
        } else if (*fmt == '%') {
            switch (*++fmt) {
                case 's':
                    const char *s = va_arg(ap, const char*);
                    if (!s) { s = "(null)"; }
                    print_s(s, con);
                    break;
                default:
                    break;
            }
        }
    }

    va_end(ap);
}