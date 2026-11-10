#include <video/vesa.h>
#include <stdint.h>
#include <string.h>
#include <bios/bios_call.h>

uint16_t get_vbe_controller_info(vbe_info_block* vbeib) {
    uint32_t addr = (uint32_t)vbeib;
 
    regs r = {0};
    r.eax   = 0x4F00;
    r.es    = addr >> 4;
    r.edi   = addr & 0xF;
    bios_call(0x10, &r);

    return r.eax & 0xFFFF;
}

uint16_t get_vbe_mode_info(int mode, vbe_mode_info *vbemi) {
    uint32_t addr = (uint32_t)vbemi;

    regs r = {0};
    r.eax   = 0x4F01;
    r.ecx   = mode;
    r.es    = addr >> 4;
    r.edi   = addr & 0xF;
    bios_call(0x10, &r);

    return r.eax & 0xFFFF;
}

uint16_t vbe_set_mode(uint16_t width, uint16_t height, uint8_t bpp, vbe_info_block *vbeib, vbe_mode_info *vbemi) {
    //  Request VBE 2.0+ info
    vbeib->signature[0] = 'V';
    vbeib->signature[1] = 'B';
    vbeib->signature[2] = 'E';
    vbeib->signature[3] = '2';

    uint16_t status = get_vbe_controller_info(vbeib);
    if (status != 0x004F)
        return status;

    //  mode_list is a far pointer (seg:off); it may point into vbeib->rest,
    //  so vbeib must stay valid while iterating.
    uint16_t *modes = (uint16_t *)(((vbeib->mode_list >> 16) << 4)
                                 + (vbeib->mode_list & 0xFFFF));

    //  Pick the closest mode: smallest resolution distance first,
    //  then smallest bpp distance as a tie-breaker.
    uint16_t best_mode     = 0xFFFF;
    uint32_t best_res_dist = 0xFFFFFFFF;
    uint32_t best_bpp_dist = 0xFFFFFFFF;

    for (; *modes != 0xFFFF; modes++) {
        uint16_t mode = *modes;

        if (get_vbe_mode_info(mode, vbemi) != 0x004F)
            continue;

        //  Supported (bit 0), graphics (bit 4), linear framebuffer (bit 7)
        if ((vbemi->attributes & 0x91) != 0x91)
            continue;

        uint32_t dw = vbemi->width  > width  ? vbemi->width  - width  : width  - vbemi->width;
        uint32_t dh = vbemi->height > height ? vbemi->height - height : height - vbemi->height;
        uint32_t db = vbemi->bpp    > bpp    ? vbemi->bpp    - bpp    : bpp    - vbemi->bpp;
        uint32_t res_dist = dw + dh;

        if (res_dist < best_res_dist ||
            (res_dist == best_res_dist && db < best_bpp_dist)) {
            best_mode     = mode;
            best_res_dist = res_dist;
            best_bpp_dist = db;

            if (res_dist == 0 && db == 0)
                break;  //  Exact match, can't do better
        }
    }

    //  No usable mode: AH = 01h (function call failed)
    if (best_mode == 0xFFFF)
        return 0x014F;

    //  Reload info for the chosen mode so the caller gets the right data
    uint16_t status_mi = get_vbe_mode_info(best_mode, vbemi);
    if (status_mi != 0x004F)
        return status_mi;

    //  Set mode, bit 14 = use linear framebuffer
    regs r = {0};
    r.eax   = 0x4F02;
    r.ebx   = best_mode | 0x4000;
    bios_call(0x10, &r);

    return r.eax & 0xFFFF;
}

int vbe_read_edid(edid_raw *edid, uint16_t block) {
    uint32_t addr = (uint32_t)edid;

    regs r = {0};
    r.eax = 0x4F15;
    bios_call(0x10, &r);
    if ((r.eax & 0xFFFF) != 0x004F) { return 0; }

    memset(&r, 0, sizeof(regs));
    r.eax = 0x4F15;
    r.ebx = 0x01;
    r.edx = block;
    r.es  = addr >> 4;
    r.edi = addr & 0xF;
    bios_call(0x10, &r);
    if ((r.eax & 0xFFFF) != 0x004F) { return 0; }

    const uint8_t *b = (const uint8_t*)edid;
    uint8_t sum = 0;
    for (int i = 0; i < 128; i++) { sum += b[i]; }
    return sum == 0;
}

int edid_valid(const edid_raw *e) {
    static const uint8_t header[8] = { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 };
    for (int i = 0; i < 8; i++)
        if (e->header[i] != header[i]) return 0;
    const uint8_t *b = (const uint8_t *)e;
    uint8_t sum = 0;
    for (int i = 0; i < 128; i++) sum += b[i];
    return sum == 0;
}

int edid_parse(const edid_raw *e, edid_info *out) {
    if (!edid_valid(e)) return 0;

    uint16_t m = (uint16_t)(e->manufacturer[0] << 8 | e->manufacturer[1]);
    out->vendor[0] = (char)('A' - 1 + ((m >> 10) & 0x1F));
    out->vendor[1] = (char)('A' - 1 + ((m >> 5) & 0x1F));
    out->vendor[2] = (char)('A' - 1 + (m & 0x1F));
    out->vendor[3] = 0;

    out->product   = e->product;
    out->year      = (uint16_t)(1990 + e->year);
    out->width_cm  = e->width_cm;
    out->height_cm = e->height_cm;

    out->width = out->height = out->refresh_hz = 0;
    const edid_timing *t = &e->descriptors[0].timing;
    if (t->pixel_clock != 0) {
        uint32_t h_active = t->h_active_lo | (uint32_t)(t->h_hi & 0xF0) << 4;
        uint32_t h_blank  = t->h_blank_lo  | (uint32_t)(t->h_hi & 0x0F) << 8;
        uint32_t v_active = t->v_active_lo | (uint32_t)(t->v_hi & 0xF0) << 4;
        uint32_t v_blank  = t->v_blank_lo  | (uint32_t)(t->v_hi & 0x0F) << 8;
        uint32_t total    = (h_active + h_blank) * (v_active + v_blank);
        out->width  = (uint16_t)h_active;
        out->height = (uint16_t)v_active;
        if (total)
            out->refresh_hz = (uint16_t)(((uint32_t)t->pixel_clock * 10000 + total / 2) / total);
    }

    out->name[0] = 0;
    for (int i = 1; i < 4; i++) {
        const edid_text *d = &e->descriptors[i].text;
        if (d->zero0 == 0 && d->zero1 == 0 && d->tag == 0xFC) {
            int n = 0;
            while (n < 13 && d->data[n] != 0x0A) {
                out->name[n] = (char)d->data[n];
                n++;
            }
            out->name[n] = 0;
            break;
        }
    }
    return 1;
}