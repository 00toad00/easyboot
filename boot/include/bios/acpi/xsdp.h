#pragma once

#include <acpi/rsdp.h>

typedef struct {
    rsdp        base;
    uint32_t    length;
    uint64_t    xsdt_addr;
    uint8_t     extended_checksum;
    uint8_t     reserved[3];
} __attribute__((packed)) xsdp;