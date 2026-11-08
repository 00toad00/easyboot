#pragma once

#include <cstdint>
#include <cstdarg>

class Console {
    public:
        virtual void putc(char c)               = 0;
        virtual void print(const char *str)     = 0;
        virtual void clear()                    = 0;

        void printf(const char *fmt, ...);
};