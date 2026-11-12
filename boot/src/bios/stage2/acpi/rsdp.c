#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <acpi/rsdp.h>
#include <acpi/xsdp.h>

static int checksum_ok(const uint8_t *p, size_t len) {
    uint8_t sum = 0;
    for (size_t i = 0; i < len; i++) sum += p[i];
    return sum == 0;
}

static int rsdp_valid(const uint8_t *p) {
    if (memcmp(p, "RSD PTR ", 8) != 0) { return 0; }
    if (!checksum_ok(p, sizeof(rsdp))) { return 0; }

    const rsdp *r = (const rsdp *)p;
    if (r->revision >= 2) {
        const xsdp *x = (const xsdp *)p;
        if (x->length < sizeof(xsdp)) { return 0; }
        if (!checksum_ok(p, x->length)) { return 0; }
    }
    return 1;
}

static void *scan(uintptr_t start, uintptr_t end) {
    for (uintptr_t a = start & ~(uintptr_t)15; a + 20 <= end; a += 16)
        if (rsdp_valid((const uint8_t *)a)) { return (void *)a; }
    return NULL;
}

void *find_rsdp(void) {
    uintptr_t ebda = (uintptr_t)(*(volatile uint16_t *)0x40E) << 4;
    void *r = NULL;

    if (ebda >= 0x80000 && ebda < 0xA0000)        // sanity check
        r = scan(ebda, ebda + 1024);
    if (!r)
        r = scan(0xE0000, 0x100000);
    return r;
}