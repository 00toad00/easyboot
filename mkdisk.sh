#!/usr/bin/env bash
# Build a bootable test disk image from build/stage-1.bin + build/stage-2.bin.
#
#   ./mkdisk.sh            -> build/disk.img
#   ./mkdisk.sh --run      -> build it and boot it in QEMU
#
# Layout:
#   LBA 0       stage 1 (MBR), patch area filled in with stage 2 location/size
#   LBA 2048    stage 2
set -euo pipefail
cd "$(dirname "$0")"

STAGE1=build/stage-1.bin
STAGE2=build/stage-2.bin
IMG=build/disk.img
IMG_SIZE_MB=16
STAGE2_LBA=2048

for f in "$STAGE1" "$STAGE2"; do
    [[ -f $f ]] || { echo "missing $f - run ./build.py first" >&2; exit 1; }
done

s1_size=$(stat -c%s "$STAGE1")
(( s1_size == 512 )) || { echo "$STAGE1 is $s1_size bytes, expected 512" >&2; exit 1; }

s2_size=$(stat -c%s "$STAGE2")
s2_sectors=$(( (s2_size + 511) / 512 ))

# Fresh, zeroed image
rm -f "$IMG"
truncate -s ${IMG_SIZE_MB}M "$IMG"

# Stage 1 into the MBR
dd if="$STAGE1" of="$IMG" bs=512 count=1 conv=notrunc status=none

# Patch area at offset 420: stage2_lba (u32 LE), stage2_size (u16 LE, sectors)
python3 -I -c 'import struct,sys; sys.stdout.buffer.write(struct.pack("<IH", int(sys.argv[1]), int(sys.argv[2])))' \
    "$STAGE2_LBA" "$s2_sectors" | dd of="$IMG" bs=1 seek=420 conv=notrunc status=none

# Stage 2
dd if="$STAGE2" of="$IMG" bs=512 seek=$STAGE2_LBA conv=notrunc status=none

echo "$IMG: stage 2 = $s2_size bytes ($s2_sectors sectors) at LBA $STAGE2_LBA"

if [[ ${1:-} == --run ]]; then
    exec qemu-system-x86_64 \
        -drive file="$IMG",format=raw \
        -no-reboot -d int,cpu_reset -D build/qemu.log
fi
