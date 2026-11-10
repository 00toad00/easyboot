#include <video/vesa.h>
#include <stdint.h>
#include <bios/bios_call.h>

uint16_t get_vbe_controller_info(vbe_info_block* vbeib) {
    uint32_t addr = (uint32_t)vbeib;
 
    regs r;
    r.eax   = 0x4F00;
    r.es    = addr >> 4;
    r.edi   = addr & 0xF;
    bios_call(0x10, &r);

    return r.eax & 0xFFFF;
}
