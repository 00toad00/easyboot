#!.venv/bin/python

from buildsys.buildsys import Target, Compiler, Build
import copy

build = Build()

compiler = Compiler()
compiler.AS         = "as"
compiler.CC         = "clang"
compiler.CXX        = "clang++"
compiler.ASFLAGS    = "--64"
compiler.CFLAGS     = "-O2 -Wall -Wextra -ffreestanding"
compiler.CXXFLAGS   = "-O2 -Wall -Wextra -ffreestanding"

target = Target()
target.TARGETNAME   = "biosstage1"
target.OUT          = "build/stage-1.bin"
target.BUILDDIR     = "build"
target.COMPILER     = copy.copy(compiler)
target.COMPILER.LDFLAGS = "-T bios/linker/stage-1.ld"

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
target.COMPILER.LDFLAGS = "-T bios/linker/stage-2.ld"

target.SRCDIR   = "bios/src/stage-2/boot"
target.SRC      = ["entry.S"]

build.addTarget(target)

build.build()