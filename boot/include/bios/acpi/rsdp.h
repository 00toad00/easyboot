#pragma once

#include <stdint.h>

typedef struct {
    char        signature[4];
    uint8_t     checksum;
    char        oem_id[6];
    uint8_t     revision;
    uint32_t    rsdt_addr;
} __attribute__((packed)) rsdp;

void *find_rsdp();