#pragma once

#include <stddef.h>

#ifdef __cplusplus 
extern "C" {
#endif

void    *memset(void *dst, int c, size_t n);
void    *memcpy(void *__restrict dst, const void *__restrict src, size_t n);

#ifdef __cplusplus
}
#endif