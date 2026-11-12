#pragma once

#include <stddef.h>

void    *memset(void *dst, int c, size_t n);
void    *memcpy(void *__restrict dst, const void *__restrict src, size_t n);
int     memcmp(const void *ptr1, const void *ptr2, size_t n);