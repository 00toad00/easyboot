#include <stdint.h>

//  libk
#include <string.h>

//  Video
#include <video/vesa.h>
#include <video/console.h>

void main() {
    //  Graphics mode initialization
    vbe_info_block vbe_info;
    vbe_mode_info vbe_mode;
    const char *sig = "VBE2";
    memcpy(vbe_info.signature, sig, 4);
    get_vbe_controller_info(&vbe_info);

    edid_raw edid_rw;
    edid_info edid_in;
    vbe_read_edid(&edid_rw, 0);
    edid_parse(&edid_rw, &edid_in);
    vbe_set_mode(edid_in.width, edid_in.height, 32, &vbe_info, &vbe_mode);

    uint32_t *pixel = (uint32_t*)vbe_mode.framebuffer;
    for (int y = 0; y < 1080; y++) {
        for (int x = 0; x < 1920; x++) {
            *(uint32_t*)(vbe_mode.framebuffer + y*edid_in.width*4 + x*(32/8)) = 0x0000FF00;
        }
    }

    while (1);
}