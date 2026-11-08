#include <stddef.h>

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