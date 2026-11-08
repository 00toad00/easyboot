#include <video/textconsole.hpp>

void TextConsole::putc(char c) {
    char *vga = (char*)0xB8000;
    vga[2*i] = c;
    vga[2*i+1] = 0x0F;
    i++;
}

void TextConsole::print(const char *str) {
    char *vga = (char*)0xB8000;
    for (int j = 0; str[j] != 0; j++) {
        if (str[j] == '\n') {
            i = (i / 80 + 1) * 80;
            continue;
        }
        vga[2*i] = str[j];
        vga[2*i+1] = 0x0F;
        i++;
    }
}

void TextConsole::clear() {
    char *vga = (char*)0xB8000;
    for (int j = 0; j < 80*25*2; j++) {
        vga[2*j] = ' ';
        vga[2*j+1] = 0x00;
    }
    i = 0;
}