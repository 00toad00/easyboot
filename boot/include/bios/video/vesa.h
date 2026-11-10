#pragma once

#include <stdint.h>

typedef struct {
    char     signature[4];
    uint16_t version;
    uint32_t oem_string;        /* far pointer */
    uint32_t capabilities;
    uint32_t mode_list;         /* far pointer */
    uint16_t total_memory;      /* × 64 KiB */
    uint8_t  rest[492];
} __attribute__((packed)) vbe_info_block;

uint16_t get_controller_info(vbe_info_block* vbeib);
