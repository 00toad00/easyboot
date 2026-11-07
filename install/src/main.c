#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        printf(argv[i]);
    }
}