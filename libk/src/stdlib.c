#include <stdint.h>

const char *chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void utoa(uint32_t i, int base, char *buf) {
    char tmp[64];                      // max 64 digits (base 2)
    int n = 0;

    if (base < 2 || base > 36) { buf[0] = '\0'; return; }

    do {
        tmp[n++] = chars[i % base];   // lowest digit first
        i /= base;
    } while (i);                       // do/while so 0 gives "0"

    while (n--) *buf++ = tmp[n];       // reverse into buf
    *buf = '\0';
}

void itoa(int32_t i, char *buf) {
    if (i < 0) {
        *buf++ = '-';
        utoa(-(uint32_t)i, 10, buf);
    } else {
        utoa((uint32_t)i, 10, buf);
    }
}