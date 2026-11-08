#include <stdint.h>

#include <bios/vesa.hpp>

extern "C" void main() {
    vbe_info_block vbe_info;
    vbe_info.signature[0] = 'V'; vbe_info.signature[1] = 'B';
    vbe_info.signature[2] = 'E'; vbe_info.signature[3] = '2';
    bios::vesa::get_controller_info(&vbe_info);

    uint8_t *vga = (uint8_t*)0xB8000;

    const char *str = "Hello World";

    for (int i = 0; str[i] != 0; i++) {
        vga[i*2] = str[i];
        vga[i*2+1] = 0x0F;
    }

    while (1);
}