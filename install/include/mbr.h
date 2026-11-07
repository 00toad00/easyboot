#pragma once

#include <stdint.h>

enum {
    MBR_TYPE_UNUSED = 0x00,
    MBR_TYPE_NOFS   = 0xDA,
    MBR_TYPE_EFI    = 0xEF
};

typedef struct __attribute__((packed)) {
    uint8_t     status;
    uint8_t     chs_first[3];
    uint8_t     type;
    uint8_t     chs_last[3];
    uint32_t    lba;
    uint32_t    size;
} MbrEntry;

typedef struct __attribute__((packed)) {
    uint8_t     code[440];
    uint32_t    disk_signature;
    uint16_t    reserved;
    MbrEntry    entry[4];
    uint16_t    signature;
} Mbr;