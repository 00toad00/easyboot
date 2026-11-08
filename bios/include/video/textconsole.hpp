#include <video/console.hpp>

#include <cstdint>
#include <cstdarg>

class TextConsole : public Console {
    private:
        int  i = 0;
    public:
        void putc(char c)           override;
        void print(const char *str) override;
        void clear()                override;
};