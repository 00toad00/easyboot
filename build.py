#!.venv/bin/python

from buildsys.buildsys import Target, Compiler, Build

compiler = Compiler()
compiler.ASFLAGS    = "--64"
compiler.CFLAGS     = "-O2 -Wall -Wextra -ffreestanding"
compiler.CXXFLAGS   = "-O2 -Wall -Wextra -ffreestanding"

target = Target()
target.TARGETNAME   = "biosstage1"
target.COMPILER     = compiler
target.OUT          = "stage-1.bin"
target.BUILDDIR     = "build"
target.COMPILER.LDFLAGS = "-T bios/linker/stage-1.ld"

target.SRCDIR   = "bios/src/stage-1"
target.SRC      = ["entry.S",
                   "disk.S"]

build = Build()
build.addTarget(target)
build.build()