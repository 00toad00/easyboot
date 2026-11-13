#include <stdint.h>

//  libk
#include <string.h>

//  Video
#include <video/vesa.h>
#include <video/framebuffer.h>
#include <video/bootlogo.h>
#include <video/console.h>

//  ACPI
#include <acpi/rsdp.h>
#include <acpi/xsdp.h>

void main() {
    //  Graphics initialization
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

    framebuffer fb;
    fb.lfb = (void*)vbe_mode.framebuffer;
    fb.width = (int)vbe_mode.width;
    fb.height = (int)vbe_mode.height;
    fb.bpp = vbe_mode.bpp;
    fb.pitch = (vbe_info.version >= 0x0300) ? vbe_mode.lin_bytes_per_scan_line : vbe_mode.bytes_per_scan_line;

    //  Boot logo
    bootlogo_draw(&fb, bootlogo, 0x0000FF00, 0x00000000);

    while (1);
}