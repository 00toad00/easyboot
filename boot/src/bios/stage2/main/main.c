#include <stdint.h>

//  libk
#include <string.h>

//  Video
#include <video/vesa.h>
#include <video/console.h>

void main() {
    //  Graphics mode initialization
    vbe_info_block vbe_info;
    const char *sig = "VBE2";
    memcpy(vbe_info.signature, sig, 4);
    get_controller_info(&vbe_info) == 0x004F;

    while (1);
}