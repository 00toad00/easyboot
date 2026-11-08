#include <stdint.h>

//  libc
#include <memory.h>

//  Video
#include <bios/vesa.hpp>
#include <video/console.hpp>
#include <video/textconsole.hpp>

extern "C" void main() {
    Console *Out;
    TextConsole tcon;
    Out = &tcon;

    Out->print("Bootloader started successfully\n");
    Out->print("VESA Initialization...");

    //  Graphics mode initialization
    vbe_info_block vbe_info;
    const char *sig = "VBE2";
    memcpy(vbe_info.signature, sig, 4);
    bios::vesa::get_controller_info(&vbe_info);

    while (1);
}