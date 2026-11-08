#!.venv/bin/python

from buildsys.buildsys import Target, Compiler, Build
import copy

build = Build()

compiler = Compiler()
compiler.AS         = "as"
compiler.CC         = "gcc"
compiler.CXX        = "g++"
compiler.ASFLAGS    = "--32"
compiler.CFLAGS     = "-O2 -Wall -Wextra -ffreestanding -m32 -fno-pic -mno-mmx -mno-sse -mno-sse2"
compiler.CXXFLAGS   = "-O2 -Wall -Wextra -ffreestanding -m32 -fno-pic -mno-mmx -mno-sse -mno-sse2 -fno-rtti -fno-exceptions"

target = Target()
target.TARGETNAME   = "libc"
target.OUT          = "build/libc.a"
target.BUILDDIR     = "build"
target.COMPILER     = copy.copy(compiler)
target.COMPILER.LD  = ""
target.COMPILER.AR  = "ar"
target.COMPILER.ARFLAGS = "rcs"

target.SRCDIR   = "libc/src"
target.SRC      = ["memory.c"]

build.addTarget(target)

target = Target()
target.TARGETNAME   = "biosstage1"
target.OUT          = "build/stage-1.bin"
target.BUILDDIR     = "build"
target.COMPILER     = copy.copy(compiler)
target.COMPILER.LDFLAGS = "-T bios/linker/stage-1.ld -m elf_i386"

target.SRCDIR   = "bios/src/stage-1"
target.SRC      = ["entry.S",
                   "disk.S",
                   "gdt.S"]

build.addTarget(target)

target = Target()
target.TARGETNAME   = "biosstage2"
target.OUT          = "build/stage-2.bin"
target.BUILDDIR     = "build"
target.COMPILER     = copy.copy(compiler)
target.COMPILER.LDFLAGS = "-T bios/linker/stage-2.ld -m elf_i386"
target.COMPILER.CPPFLAGS = "-Ibios/include -Ilibc/include"
target.COMPILER.LIBS = "build/libc.a"

target.SRCDIR   = "bios/src/stage-2/"
target.SRC      = ["boot/entry.S",
                   "boot/main.cpp",
                   
                   "bios/vesa.S",
                   "video/textconsole.cpp"]

build.addTarget(target)

build.build()