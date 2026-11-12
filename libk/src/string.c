#include <stddef.h>
#include <stdint.h>

void *memset(void *dst, int c, size_t n) {
    for (size_t i = 0; i < n; i++) {
        ((char*)dst)[i] = (char)c;
    }
    return dst;
}

void *memcpy(void *__restrict dst, const void *__restrict src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        ((char*)dst)[i] = ((char*)src)[i];
    }
    return dst;
}

int memcmp(const void *ptr1, const void *ptr2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (((uint8_t*)ptr1)[i] < ((uint8_t*)ptr2)[i]) { return -1; }
        if (((uint8_t*)ptr1)[i] > ((uint8_t*)ptr2)[i]) { return 1; }
    }
    return 0;
}