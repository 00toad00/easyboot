# turtle-kernel build
#
#   make            build everything (stage 1, stage 2, libk)
#   make disk       build the bootable image build/disk.img
#   make run        build the image and boot it in QEMU
#   make V=1        show full commands
#   make clean      remove the build directory
#
# Sources (*.c, *.S, *.s) are discovered automatically under each component's
# source directory. Objects go to $(BUILD)/<path of source>.o, mirroring the
# source tree. Header dependencies are tracked via -MMD/-MP.

# --------------------------------------------------------------------------
#  Toolchain / global flags
# --------------------------------------------------------------------------
BUILD    ?= build

# make predefines CC=cc; prefer gcc unless overridden on the command line/env
ifeq ($(origin CC),default)
CC       := gcc
endif
LD       ?= ld
AR       ?= ar

ARCH_CFLAGS  := -m32 -fno-pic -fno-pie -mno-mmx -mno-sse -mno-sse2 -mno-80387
CFLAGS       ?= -O2 -g
CFLAGS       += $(ARCH_CFLAGS) -std=gnu11 -Wall -Wextra \
                -ffreestanding -fno-builtin -fno-stack-protector \
                -fno-asynchronous-unwind-tables -fno-unwind-tables \
				-fno-delete-null-pointer-checks --param=min-pagesize=0
ASFLAGS      += -m32
CPPFLAGS     +=
DEPFLAGS      = -MMD -MP -MF $(@:.o=.d) -MT $@
LDFLAGS      += -m elf_i386 -nostdlib
ARFLAGS      := rcs

# Quiet output unless V=1
ifeq ($(V),1)
Q :=
msg = @true
else
Q := @
msg = @printf '  %-6s %s\n' $(1) $(2)
endif

# --------------------------------------------------------------------------
#  Helpers
# --------------------------------------------------------------------------
# All C / assembly sources below a directory, sorted for stable builds.
find_srcs = $(sort $(shell find $(1) -type f \( -name '*.c' -o -name '*.S' -o -name '*.s' \) 2>/dev/null))
# src/foo/bar.c -> $(BUILD)/src/foo/bar.o
to_objs   = $(addprefix $(BUILD)/,$(addsuffix .o,$(basename $(1))))
# Put any entry.o first so its code lands at the start of .text.
entry_first = $(filter %/entry.o,$(1)) $(filter-out %/entry.o,$(1))

# --------------------------------------------------------------------------
#  libk  (static library, linked into stage 2)
# --------------------------------------------------------------------------
LIBK_DIR    := libk
LIBK_SRCS   := $(call find_srcs,$(LIBK_DIR)/src)
LIBK_OBJS   := $(call to_objs,$(LIBK_SRCS))
LIBK        := $(BUILD)/libk.a

$(LIBK_OBJS): CPPFLAGS += -I$(LIBK_DIR)/include

# --------------------------------------------------------------------------
#  BIOS stage 1  (MBR)
# --------------------------------------------------------------------------
STAGE1_DIR  := boot/src/bios/stage1
STAGE1_SRCS := $(call find_srcs,$(STAGE1_DIR))
STAGE1_OBJS := $(call to_objs,$(STAGE1_SRCS))
STAGE1_LDS  := $(STAGE1_DIR)/stage1.ld
STAGE1      := $(BUILD)/stage-1.bin

$(STAGE1_OBJS): CPPFLAGS += -Iboot/include -Iboot/include/bios

# --------------------------------------------------------------------------
#  BIOS stage 2
# --------------------------------------------------------------------------
STAGE2_DIR  := boot/src/bios/stage2
STAGE2_SRCS := $(call find_srcs,$(STAGE2_DIR))
STAGE2_OBJS := $(call to_objs,$(STAGE2_SRCS))
STAGE2_LDS  := $(STAGE2_DIR)/stage2.ld
STAGE2      := $(BUILD)/stage-2.bin

$(STAGE2_OBJS): CPPFLAGS += -Iboot/include -Iboot/include/bios -I$(LIBK_DIR)/include

# --------------------------------------------------------------------------
#  Top-level targets
# --------------------------------------------------------------------------
ALL_OBJS := $(LIBK_OBJS) $(STAGE1_OBJS) $(STAGE2_OBJS)
DEPS     := $(ALL_OBJS:.o=.d)

.PHONY: all libk stage1 stage2 disk run clean
.DEFAULT_GOAL := all

all: $(STAGE1) $(STAGE2)
libk: $(LIBK)
stage1: $(STAGE1)
stage2: $(STAGE2)

$(LIBK): $(LIBK_OBJS)
	$(call msg,AR,$@)
	@mkdir -p $(@D)
	$(Q)rm -f $@
	$(Q)$(AR) $(ARFLAGS) $@ $^

$(STAGE1): $(STAGE1_OBJS) $(STAGE1_LDS)
	$(call msg,LD,$@)
	@mkdir -p $(@D)
	$(Q)$(LD) $(LDFLAGS) -T $(STAGE1_LDS) -Map=$(@:.bin=.map) \
		-o $@ $(call entry_first,$(STAGE1_OBJS))

$(STAGE2): $(STAGE2_OBJS) $(LIBK) $(STAGE2_LDS)
	$(call msg,LD,$@)
	@mkdir -p $(@D)
	$(Q)$(LD) $(LDFLAGS) -T $(STAGE2_LDS) -Map=$(@:.bin=.map) \
		-o $@ $(call entry_first,$(STAGE2_OBJS)) $(LIBK)

# --------------------------------------------------------------------------
#  Pattern rules (output mirrors source tree under $(BUILD))
#  Objects also depend on this Makefile so flag changes trigger a rebuild.
# --------------------------------------------------------------------------
$(BUILD)/%.o: %.c Makefile
	$(call msg,CC,$<)
	@mkdir -p $(@D)
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# .S goes through the C preprocessor, so it gets dependency tracking too
$(BUILD)/%.o: %.S Makefile
	$(call msg,AS,$<)
	@mkdir -p $(@D)
	$(Q)$(CC) $(CPPFLAGS) $(ASFLAGS) $(DEPFLAGS) -c $< -o $@

# .s is plain assembly (no preprocessor, no includes to track)
$(BUILD)/%.o: %.s Makefile
	$(call msg,AS,$<)
	@mkdir -p $(@D)
	$(Q)$(CC) $(ASFLAGS) -c $< -o $@

# --------------------------------------------------------------------------
#  Disk image + QEMU (same layout as util/mkdisk.sh)
#    LBA 0     stage 1 (MBR); patch area at offset 420 gets
#              stage2_lba (u32 LE) and stage2_size (u16 LE, sectors)
#    LBA 2048  stage 2
# --------------------------------------------------------------------------
IMG          := $(BUILD)/disk.img
IMG_SIZE_MB  ?= 16
STAGE2_LBA   ?= 2048
QEMU         ?= qemu-system-x86_64
QEMUFLAGS    ?= -no-reboot -d int,cpu_reset -D $(BUILD)/qemu.log

disk: $(IMG)

$(IMG): $(STAGE1) $(STAGE2) Makefile
	$(call msg,IMG,$@)
	$(Q)s1=$$(stat -c%s $(STAGE1)); \
	if [ "$$s1" -ne 512 ]; then \
		echo "$(STAGE1) is $$s1 bytes, expected 512" >&2; exit 1; \
	fi; \
	s2=$$(stat -c%s $(STAGE2)); \
	sectors=$$(( (s2 + 511) / 512 )); \
	rm -f $@ && truncate -s $(IMG_SIZE_MB)M $@ && \
	dd if=$(STAGE1) of=$@ bs=512 count=1 conv=notrunc status=none && \
	python3 -I -c 'import struct,sys; sys.stdout.buffer.write(struct.pack("<IH", int(sys.argv[1]), int(sys.argv[2])))' \
		$(STAGE2_LBA) $$sectors | dd of=$@ bs=1 seek=420 conv=notrunc status=none && \
	dd if=$(STAGE2) of=$@ bs=512 seek=$(STAGE2_LBA) conv=notrunc status=none && \
	echo "  $@: stage 2 = $$s2 bytes ($$sectors sectors) at LBA $(STAGE2_LBA)" || \
	{ rm -f $@; exit 1; }

run: $(IMG)
	$(QEMU) -drive file=$(IMG),format=raw $(QEMUFLAGS)

clean:
	$(call msg,CLEAN,$(BUILD))
	$(Q)rm -rf $(BUILD)

-include $(DEPS)
