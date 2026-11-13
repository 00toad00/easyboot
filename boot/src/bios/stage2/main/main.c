#include <stdint.h>

//  libk
#include <string.h>
#include <stdlib.h>

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

    console con;
    con.fb = &fb;
    con.border.x = 10; con.border.y = 10;
    con.pos.x = 0; con.pos.y = 0;
    con.glyph.x = 8; con.glyph.y = 13;
    con.pad.x = 1; con.pad.y = 1;

    //  Boot logo
    bootlogo_draw(&fb, bootlogo, 0xFFFFFFFF, 0x00000000);

    while (1);
}