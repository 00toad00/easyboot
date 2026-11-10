#include <video/console.h>

void putc(console *con, char c) {
    char *vga = con->vga;
    vga[2*con->i] = c;
    vga[2*con->i+1] = 0x0F;
    con->i++;
}

void print(console *con, const char *str) {
    char *vga = con->vga;
    for (int j = 0; str[j] != 0; j++) {
        if (str[j] == '\n') {
            con->i = (con->i / 80 + 1) * 80;
            continue;
        }
        vga[2*con->i] = str[j];
        vga[2*con->i+1] = 0x0F;
        con->i++;
    }
}

void clear(console *con) {
    char *vga = con->vga;
    for (int j = 0; j < 80*25*2; j++) {
        vga[2*j] = ' ';
        vga[2*j+1] = 0x00;
    }
}