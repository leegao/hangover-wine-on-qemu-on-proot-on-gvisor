#!/usr/bin/env bash
set -euo pipefail

KEEP_32BIT="${KEEP_32BIT:-0}"
PATCH="${PATCH:-1}"
GFX_STUB="${GFX_STUB:-1}"
RAW_INPUT=""

for arg in "$@"; do
    case "${arg}" in
        --keep-32bit)
            KEEP_32BIT=1
            echo "Keeping 32bit wine components"
            ;;
        --no-patch)
            PATCH=0
            echo "Not patching libraries for compatibility with qemu-aarch64-static/proot"
            ;;
        --no-gfx-stub) # TODO(leegao): replace with real lavapipe
            GFX_STUB=0
            echo "Not adding GL and Vulkan stubs (can only run headless)"
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

if [ -f "${INPUT_DIR}/proot-qemu.zip" ]; then
    sudo unzip -q -o "${INPUT_DIR}/proot-qemu.zip" -d "${PROOT_DIR}"
fi
chmod +x "${PROOT_DIR}/proot" "${PROOT_DIR}/qemu-aarch64-static" 2>/dev/null || true

LLVM_TAR="$(ls "${INPUT_DIR}"/llvm-mingw*.tar.xz 2>/dev/null | head -n 1 || true)"
if [ -n "${LLVM_TAR}" ] && [ -f "${LLVM_TAR}" ]; then
    chmod -R u+w "${OUTPUT_DIR}"/llvm-mingw* 2>/dev/null || true
    tar -xf "${LLVM_TAR}" -C "${OUTPUT_DIR}" --no-same-owner
fi
LLVM_DIR="$(ls -d "${OUTPUT_DIR}"/llvm-mingw* 2>/dev/null | head -n 1)"
CLANG="${LLVM_DIR}/bin/clang"
CLANG_MINGW="${LLVM_DIR}/bin/x86_64-w64-mingw32-clang"
OBJCOPY_MINGW="${LLVM_DIR}/bin/aarch64-w64-mingw32-objcopy"
OBJCOPY_32="${LLVM_DIR}/bin/i686-w64-mingw32-objcopy"

if [ -f "${INPUT_DIR}/debian-arm64-rootfs.zip" ]; then
    ROOTFS_ZIP="${INPUT_DIR}/debian-arm64-rootfs.zip"
    sudo unzip -q -o "${ROOTFS_ZIP}" -d "${ROOTFS}"
elif [ -f "${INPUT_DIR}/hangover-proot-qemu.tar.xz" ]; then
    chmod -R u+w "${ROOTFS}" 2>/dev/null || true
    tar -xf "${INPUT_DIR}/hangover-proot-qemu.tar.xz" -C "${OUTPUT_DIR}" --no-same-owner
elif [ -f "${INPUT_DIR}/hangover-proot-qemu.tar" ]; then
    chmod -R u+w "${ROOTFS}" 2>/dev/null || true
    tar -xf "${INPUT_DIR}/hangover-proot-qemu.tar" -C "${OUTPUT_DIR}" --no-same-owner
else
    echo "Warning: debian-arm64-rootfs archive not found in ${INPUT_DIR}" >&2
fi

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

# Extract DXVK if provided in output/ or inputs/
DXVK_TAR="$(ls "${OUTPUT_DIR}"/dxvk*.tar.gz "${INPUT_DIR}"/dxvk*.tar.gz 2>/dev/null | head -n 1 || true)"
if [ -n "${DXVK_TAR}" ] && [ -f "${DXVK_TAR}" ]; then
    echo "  Unpacking DXVK from $(basename "${DXVK_TAR}")..."
    tar -xzf "${DXVK_TAR}" -C "${OUTPUT_DIR}"
    DXVK_DIR="$(find "${OUTPUT_DIR}" -maxdepth 1 -type d -name "dxvk-*" | head -n 1)"
    mkdir -p "${OUTPUT_DIR}/dxvk"
    if [ -n "${DXVK_DIR}" ] && [ -d "${DXVK_DIR}/arm64ec" ]; then
        cp -f "${DXVK_DIR}/arm64ec"/*.dll "${OUTPUT_DIR}/dxvk/"
    elif [ -n "${DXVK_DIR}" ] && [ -d "${DXVK_DIR}/x64" ]; then
        cp -f "${DXVK_DIR}/x64"/*.dll "${OUTPUT_DIR}/dxvk/"
    fi
    if [ -n "${DXVK_DIR}" ]; then
        rm -rf "${DXVK_DIR}"
    fi
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

if [ -d "${ROOTFS}/usr/lib/wine/aarch64-windows" ]; then
    find "${ROOTFS}/usr/lib/wine/aarch64-windows" -maxdepth 1 -type f \( -name "*.dll" -o -name "*.exe" \) \
        ! -name "wineboot.exe" ! -name "libarm64ecfex.dll" ! -name "libwow64fex.dll" -print0 | \
        xargs -0 -P 8 -n 1 "${OBJCOPY_MINGW}" \
            --remove-section=.debug_info --remove-section=.debug_abbrev \
            --remove-section=.debug_line --remove-section=.debug_str \
            --remove-section=.debug_aranges --remove-section=.debug_ranges \
            --remove-section=.debug_loc --remove-section=.debug_frame 2>/dev/null || true
fi

rm -f "${ROOTFS}/usr/lib/wine/aarch64-windows"/*.a 2>/dev/null || true
rm -rf "${ROOTFS}/usr/include" "${ROOTFS}/var/cache/apt"/* "${ROOTFS}/var/lib/apt/lists"/* 2>/dev/null || true
rm -rf "${ROOTFS}/usr/libexec/gcc" "${ROOTFS}/usr/lib/gcc" 2>/dev/null || true
rm -f "${ROOTFS}/usr/bin"/*lto-dump* "${ROOTFS}/usr/bin"/sq* "${ROOTFS}/usr/bin"/qemu-aarch64-static 2>/dev/null || true
rm -rf "${ROOTFS}/usr/share/doc"/* "${ROOTFS}/usr/share/man"/* "${ROOTFS}/usr/share/locale"/* 2>/dev/null || true

mkdir -p "${ROOTFS}/usr/lib/aarch64-linux-gnu"
if [ -f "${OUTPUT_DIR}/headless_stubs.s" ] && [ -x "${CLANG}" ]; then
    "${CLANG}" --target=aarch64-linux-gnu -shared -nostdlib -fuse-ld=lld \
        -o "${ROOTFS}/usr/lib/aarch64-linux-gnu/libwine_headless_stubs.so" \
        "${OUTPUT_DIR}/headless_stubs.s"
fi

mkdir -p "${ROOTFS}/host-rootfs"
chmod 0755 "${ROOTFS}/host-rootfs"
PRELOAD="${ROOTFS}/etc/ld.so.preload"
if [ ! -f "${PRELOAD}" ]; then
    touch "${PRELOAD}"
fi
chmod 0644 "${PRELOAD}"

WINE_DIR="${ROOTFS}/root/.wine"
DOSDEVICES="${WINE_DIR}/dosdevices"
SYSTEM32="${WINE_DIR}/drive_c/windows/system32"
mkdir -p "${DOSDEVICES}" "${SYSTEM32}"
C_LINK="${DOSDEVICES}/c:"
Z_LINK="${DOSDEVICES}/z:"
if [ ! -e "${C_LINK}" ]; then
    ln -s "../drive_c" "${C_LINK}"
fi
if [ ! -e "${Z_LINK}" ]; then
    ln -s "/" "${Z_LINK}"
fi

if [ "${GFX_STUB}" = "1" ]; then
    # DT_NEEDED symlinks -> libwine_headless_stubs.so
    LIB_DIR="${ROOTFS}/usr/lib/aarch64-linux-gnu"
    if [ -d "${LIB_DIR}" ]; then
        for s in \
            libsane.so.1 \
            libasound.so.2 \
            libgstaudio-1.0.so.0 \
            libgstvideo-1.0.so.0 \
            libgstbase-1.0.so.0 \
            libgstreamer-1.0.so.0 \
            libgobject-2.0.so.0 \
            libglib-2.0.so.0 \
            libgsttag-1.0.so.0 \
            libpulse.so.0 \
            libusb-1.0.so.0 \
            libwayland-egl.so.1 \
            libwayland-client.so.0 \
            libxkbcommon.so.0 \
            libXext.so.6 \
            libX11.so.6; do
            dst="${LIB_DIR}/${s}"
            if [ ! -e "${dst}" ]; then
                ln -s "libwine_headless_stubs.so" "${dst}"
            fi
        done
    fi

    if [ -f "${OUTPUT_DIR}/headless_gl.c" ] && [ -x "${CLANG}" ]; then
        "${CLANG}" --target=aarch64-linux-gnu -ffreestanding -O2 -shared -fPIC -nostdlib -fuse-ld=lld \
            -Wl,-soname,libEGL.so.1 \
            "${OUTPUT_DIR}/headless_gl.c" \
            "${ROOTFS}/usr/lib/aarch64-linux-gnu/libc.so.6" \
            -o "${ROOTFS}/usr/lib/aarch64-linux-gnu/libEGL.so.1"
        ln -sf libEGL.so.1 "${ROOTFS}/usr/lib/aarch64-linux-gnu/libGL.so.1"
        echo "EGL stub: ${ROOTFS}/usr/lib/aarch64-linux-gnu/libEGL.so.1"
    fi

    if [ -f "${OUTPUT_DIR}/headless_vk.c" ] && [ -x "${CLANG}" ]; then
        "${CLANG}" --target=aarch64-linux-gnu -ffreestanding -O2 -shared -fPIC -nostdlib -fuse-ld=lld \
            -Wl,-soname,libvulkan.so.1 \
            "${OUTPUT_DIR}/headless_vk.c" \
            "${ROOTFS}/usr/lib/aarch64-linux-gnu/libc.so.6" \
            -o "${ROOTFS}/usr/lib/aarch64-linux-gnu/libvulkan.so.1"
        echo "Vulkan stub: ${ROOTFS}/usr/lib/aarch64-linux-gnu/libvulkan.so.1"
    fi

    USER_REG="${WINE_DIR}/user.reg"
    if [ -f "${USER_REG}" ]; then
        if ! grep -qF '[Software\\Wine\\Drivers]' "${USER_REG}"; then
            printf '\n[Software\\\\Wine\\\\Drivers] 1762205900\n#time=1db2f3b5b2a0000\n"Graphics"="null"\n' >> "${USER_REG}"
        fi
    else
        mkdir -p "${WINE_DIR}"
        cat > "${USER_REG}" << EOF
WINE REGISTRY Version 2
;; All keys relative to \\User\\S-1-5-21-0-0-0-1000

#arch=win64

[Software\\Wine\\Drivers] 1762205900
#time=1db2f3b5b2a0000
"Graphics"="null"
EOF
    fi
fi

WIN_PE="${ROOTFS}/usr/lib/wine/aarch64-windows"
if [ -d "${WIN_PE}" ]; then
    for f in "${WIN_PE}"/*; do
        [ -e "${f}" ] || continue
        dst="${SYSTEM32}/$(basename "${f}")"
        if [ ! -e "${dst}" ]; then
            ln -s "/usr/lib/wine/aarch64-windows/$(basename "${f}")" "${dst}"
        fi
    done
fi

WIN32_PE="${ROOTFS}/usr/lib/wine/i386-windows"
if [ -d "${WIN32_PE}" ]; then
    SYSWOW64="${WINE_DIR}/drive_c/windows/syswow64"
    mkdir -p "${SYSWOW64}"
    for f in "${WIN32_PE}"/*; do
        [ -e "${f}" ] || continue
        dst="${SYSWOW64}/$(basename "${f}")"
        if [ ! -e "${dst}" ]; then
            ln -s "/usr/lib/wine/i386-windows/$(basename "${f}")" "${dst}"
        fi
    done
fi

if [ "${PATCH}" = "1" ] && [ -f "${OUTPUT_DIR}/apply_patches.py" ]; then
    python3 "${OUTPUT_DIR}/apply_patches.py"
fi

mkdir -p "${ROOTFS}/tmp"
chmod 1777 "${ROOTFS}/tmp"

if [ -x "${CLANG_MINGW}" ]; then
    "${CLANG_MINGW}" -O2 "${OUTPUT_DIR}/hello.c" -o "${ROOTFS}/tmp/hello.exe"
    "${CLANG_MINGW}" -O2 -msse2 "${OUTPUT_DIR}/fex_arm64ec_test.c" -o "${ROOTFS}/tmp/fex_arm64ec_test.exe"
fi

if [ -f "${PROOT_DIR}/d3d11_bc7_sample.exe" ]; then
    cp -f "${PROOT_DIR}/d3d11_bc7_sample.exe" "${ROOTFS}/tmp/d3d11_bc7_sample.exe"
elif [ -f "${OUTPUT_DIR}/d3d11_bc7_sample.exe" ]; then
    cp -f "${OUTPUT_DIR}/d3d11_bc7_sample.exe" "${ROOTFS}/tmp/d3d11_bc7_sample.exe"
fi

echo " BUILD SUCCESSFUL!"
echo " Rootfs uncompressed size: $(du -sh "${ROOTFS}" | cut -f1)"
echo " Run any x86-64 binary with: ${OUTPUT_DIR}/run_headless_exe.sh <binary.exe>"
