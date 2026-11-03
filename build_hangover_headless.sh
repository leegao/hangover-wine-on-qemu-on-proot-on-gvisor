#!/usr/bin/env bash
set -euo pipefail

KEEP_32BIT="${KEEP_32BIT:-0}"
PATCH="${PATCH:-1}"
RAW_INPUT=""

for arg in "$@"; do
    case "${arg}" in
        --keep-32bit)
            KEEP_32BIT=1
            echo "Keeping 32bit wine components"
            ;;
        --no-patch)
            PATCH=0
            echo "Not patching libraries"
            ;;
        -h|--help)
            echo "Usage: $0 [--keep-32bit|--no-patch] [input-directory]"
            echo "  --keep-32bit   Keep 32-bit Wine subsystem (usr/lib/wine/i386-windows)"
            echo "  --no-patch     Don't patch any of the libraries"
            exit 0
            ;;
        *) [ -z "${RAW_INPUT}" ] && RAW_INPUT="${arg}" ;;
    esac
done

INPUT_DIR="$(cd "${RAW_INPUT}" && pwd)"
OUTPUT_DIR="$(pwd)/output"
ROOTFS="${OUTPUT_DIR}/debian-arm64-rootfs"
PROOT_DIR="${OUTPUT_DIR}/proot-qemu"

mkdir -p "${OUTPUT_DIR}"
cd "${OUTPUT_DIR}"

echo "Extracting proot, llvm-mingw and QEMU..."

sudo unzip -q -o "${INPUT_DIR}/proot-qemu.zip" -d "${PROOT_DIR}"
chmod +x "${PROOT_DIR}/proot" "${PROOT_DIR}/qemu-aarch64-static"

LLVM_TAR="$(ls "${INPUT_DIR}"/llvm-mingw*.tar.xz 2>/dev/null | head -n 1)"
tar -xf "${LLVM_TAR}" -C "${OUTPUT_DIR}"
LLVM_DIR="$(ls -d "${OUTPUT_DIR}"/llvm-mingw* | head -n 1)"
CLANG="${LLVM_DIR}/bin/clang"
CLANG_MINGW="${LLVM_DIR}/bin/x86_64-w64-mingw32-clang"
OBJCOPY_MINGW="${LLVM_DIR}/bin/aarch64-w64-mingw32-objcopy"
OBJCOPY_32="${LLVM_DIR}/bin/i686-w64-mingw32-objcopy"

if [ -f "${INPUT_DIR}/debian-arm64-rootfs.zip" ]; then
    ROOTFS_ZIP="${INPUT_DIR}/debian-arm64-rootfs.zip"
else
    echo "Error: debian-arm64-rootfs archive not found in ${INPUT_DIR}" >&2
    exit 1
fi

sudo unzip -q -o "${ROOTFS_ZIP}" -d "${ROOTFS}"
if [ -d "${ROOTFS}/debian-arm64-rootfs" ]; then
    echo "  Flattening nested rootfs directory..."
    mv "${ROOTFS}/debian-arm64-rootfs"/* "${ROOTFS}/"
    rmdir "${ROOTFS}/debian-arm64-rootfs"
fi

if [ -d "${ROOTFS}/tmp/assets" ]; then
    for deb in "${ROOTFS}"/tmp/assets/*.deb; do
        if [ -f "${deb}" ]; then
            echo "  Unpacking $(basename "${deb}")..."
            dpkg-deb -x "${deb}" "${ROOTFS}"
        fi
    done
    rm -rf "${ROOTFS}/tmp/assets"
fi

if [ "${KEEP_32BIT}" = "1" ]; then
    if [ -d "${ROOTFS}/usr/lib/wine/i386-windows" ]; then
        find "${ROOTFS}/usr/lib/wine/i386-windows" -maxdepth 1 -type f \( -name "*.dll" -o -name "*.exe" \) -print0 | \
            xargs -0 -P 8 -n 1 "${OBJCOPY_32}" \
                --remove-section=.debug_info --remove-section=.debug_abbrev \
                --remove-section=.debug_line --remove-section=.debug_str \
                --remove-section=.debug_aranges --remove-section=.debug_ranges \
                --remove-section=.debug_loc --remove-section=.debug_frame 2>/dev/null || true
    fi
else
    rm -rf "${ROOTFS}/usr/lib/wine/i386-windows"
fi

find "${ROOTFS}/usr/lib/wine/aarch64-windows" -maxdepth 1 -type f \( -name "*.dll" -o -name "*.exe" \) \
    ! -name "wineboot.exe" ! -name "libarm64ecfex.dll" ! -name "libwow64fex.dll" -print0 | \
    xargs -0 -P 8 -n 1 "${OBJCOPY_MINGW}" \
        --remove-section=.debug_info --remove-section=.debug_abbrev \
        --remove-section=.debug_line --remove-section=.debug_str \
        --remove-section=.debug_aranges --remove-section=.debug_ranges \
        --remove-section=.debug_loc --remove-section=.debug_frame 2>/dev/null || true

rm -f "${ROOTFS}/usr/lib/wine/aarch64-windows"/*.a
rm -rf "${ROOTFS}/usr/include" "${ROOTFS}/var/cache/apt"/* "${ROOTFS}/var/lib/apt/lists"/*
rm -rf "${ROOTFS}/usr/libexec/gcc" "${ROOTFS}/usr/lib/gcc"
rm -f "${ROOTFS}/usr/bin"/*lto-dump* "${ROOTFS}/usr/bin"/sq* "${ROOTFS}/usr/bin"/qemu-aarch64-static
rm -rf "${ROOTFS}/usr/share/doc"/* "${ROOTFS}/usr/share/man"/* "${ROOTFS}/usr/share/locale"/* 2>/dev/null || true

mkdir -p "${ROOTFS}/usr/lib/aarch64-linux-gnu"
"${CLANG}" --target=aarch64-linux-gnu -shared -nostdlib -fuse-ld=lld \
    -o "${ROOTFS}/usr/lib/aarch64-linux-gnu/libwine_headless_stubs.so" \
    "${OUTPUT_DIR}/headless_stubs.s"

if [ "${PATCH}" = "1" ]; then
    python3 "${OUTPUT_DIR}/apply_patches.py"
fi

"${CLANG_MINGW}" -O2 "${OUTPUT_DIR}/hello.c" -o "${ROOTFS}/tmp/hello.exe"
"${CLANG_MINGW}" -O2 -msse2 "${OUTPUT_DIR}/fex_arm64ec_test.c" -o "${ROOTFS}/tmp/fex_arm64ec_test.exe"

echo " BUILD SUCCESSFUL!"
echo " Rootfs uncompressed size: $(du -sh "${ROOTFS}" | cut -f1)"
echo " Run any x86-64 binary with: ${OUTPUT_DIR}/run_headless_exe.sh <binary.exe>"
