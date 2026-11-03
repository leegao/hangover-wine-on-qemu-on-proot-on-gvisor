#!/usr/bin/env python3
import os, sys

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
if os.path.exists(os.path.join(BASE_DIR, "debian-arm64-rootfs")):
    ROOTFS = os.path.join(BASE_DIR, "debian-arm64-rootfs")
    PROOT_QEMU_DIR = os.path.join(BASE_DIR, "proot-qemu")
elif os.path.exists(os.path.join(os.path.dirname(BASE_DIR), "debian-arm64-rootfs")):
    ROOTFS = os.path.join(os.path.dirname(BASE_DIR), "debian-arm64-rootfs")
    PROOT_QEMU_DIR = os.path.join(os.path.dirname(BASE_DIR), "proot-qemu")
else:
    ROOTFS = os.path.join(BASE_DIR, "debian-arm64-rootfs")
    PROOT_QEMU_DIR = os.path.join(BASE_DIR, "proot-qemu")

def patch_bytes(path, offset, orig_hex, patch_hex, desc):
    orig, patch = bytes.fromhex(orig_hex), bytes.fromhex(patch_hex)
    assert len(orig) == len(patch)
    if not os.path.exists(path):
        print(f"[SKIP] {path} not found for {desc}"); return
    with open(path, "rb") as f: data = bytearray(f.read())
    if offset + len(orig) > len(data):
        raise RuntimeError(f"File {path} too short for patch at 0x{offset:x}")
    cur = bytes(data[offset:offset+len(orig)])
    if cur == patch:
        print(f"[OK] Already patched: {desc}"); return
    if cur != orig:
        raise RuntimeError(f"Unexpected bytes in {path} at 0x{offset:x}: got {cur.hex()}, expected {orig.hex()}")
    data[offset:offset+len(patch)] = patch
    with open(path, "wb") as f: f.write(data)
    print(f"[PATCHED] {desc}")

def run():
    # 1. Disable wine-preloader
    for p in ["usr/bin/wine-preloader", "usr/lib/wine/aarch64-unix/wine-preloader"]:
        full = os.path.join(ROOTFS, p)
        if os.path.exists(full) and not os.path.islink(full): os.rename(full, full + ".bak")

    # 2. Patch ntdll.so
    ntdll = os.path.join(ROOTFS, "usr/lib/wine/aarch64-unix/ntdll.so")
    patch_bytes(ntdll, 0x4DFC0, "a00438d5", "000080d2", "ntdll.so: mrs x0, ID_AA64SMFR0_EL1 -> mov x0, #0")
    patch_bytes(ntdll, 0x4E100, "400738d5", "000080d2", "ntdll.so: mrs x0, ID_AA64MMFR2_EL1 -> mov x0, #0")
    patch_bytes(ntdll, 0x5EC54, "000200f0", "1c000014", "ntdll.so: try_map_free_area -> b 0x5ecc4")
    patch_bytes(ntdll, 0x61F48, "2000a0d2", "0000a4d2", "ntdll.so: low reserve start -> 0x20000000")
    patch_bytes(ntdll, 0x61F5C, "e17b70b2", "e17770b2", "ntdll.so: high reserve end -> 0x3fffffff0000")
    patch_bytes(ntdll, 0x61F60, "e05767b2", "e05367b2", "ntdll.so: high reserve start -> 0x3ffffe000000")
    for off, name in [(0x912F0, "address_space_limit"), (0x912F8, "user_space_limit"), (0x91300, "working_set_limit")]:
        patch_bytes(ntdll, off, "0000ffffff7f0000", "0000ffffff3f0000", f"ntdll.so .data: {name}")

    orig_pattern = bytes.fromhex("7f0006eb230100540a000014650440b9")
    patched_pattern = bytes.fromhex("c40240b99f1c0071001e005409000014")
    target_off = 0x3C530
    if os.path.exists(ntdll):
        with open(ntdll, "rb") as f: ndata = bytearray(f.read())
        if ndata[target_off:target_off+16] == patched_pattern:
            print("[OK] Already patched: ntdll.so SIGBUS -> STATUS_DATATYPE_MISALIGNMENT")
        elif ndata[target_off:target_off+16] == orig_pattern:
            patch_bytes(ntdll, target_off, orig_pattern.hex(), patched_pattern.hex(),
                        "ntdll.so: map SIGBUS directly to STATUS_DATATYPE_MISALIGNMENT")
        else:
            idx = ndata.find(orig_pattern[:8])
            if idx != -1:
                patch_bytes(ntdll, idx, orig_pattern.hex(), patched_pattern.hex(),
                            "ntdll.so: map SIGBUS directly to STATUS_DATATYPE_MISALIGNMENT (dynamic offset)")

    # 3. Patch wineboot.exe fast exit
    patch_bytes(os.path.join(ROOTFS, "usr/lib/wine/aarch64-windows/wineboot.exe"),
                0x1260C, "00e4006f", "d3080014", "wineboot.exe: fast exit after registry init")

    # 4. Patch libarm64ecfex.dll UAF
    patch_bytes(os.path.join(ROOTFS, "usr/lib/wine/aarch64-windows/libarm64ecfex.dll"),
                0x3068, "f5700694", "1f2003d5", "libarm64ecfex.dll: NOP premature aligned_free (UAF fix)")

    # 5. Patch qemu-aarch64-static
    qemu_bin = os.path.join(PROOT_QEMU_DIR, "qemu-aarch64-static")
    patch_bytes(qemu_bin, 0x21F0E7, "0f84d0020000", "909090909090",
                "qemu-aarch64-static: bypass REG_TRAPNO for JIT writes")
    orig_qemu = b"ID_AA64ISAR*_EL1_RESERVED\x00"
    patch_qemu = b"ID_AA64*\x00" + b"\x00" * (len(orig_qemu) - len(b"ID_AA64*\x00"))
    patch_bytes(qemu_bin, 0x4660E9, orig_qemu.hex(), patch_qemu.hex(),
                "qemu-aarch64-static: universal ID_AA64* CPUID user-mode read whitelist")

    # 6. DT_NEEDED symlinks
    lib_dir = os.path.join(ROOTFS, "usr/lib/aarch64-linux-gnu")
    for s in ["libsane.so.1", "libasound.so.2", "libgstaudio-1.0.so.0", "libgstvideo-1.0.so.0",
              "libgstbase-1.0.so.0", "libgstreamer-1.0.so.0", "libgobject-2.0.so.0", "libglib-2.0.so.0",
              "libgsttag-1.0.so.0", "libpulse.so.0", "libusb-1.0.so.0", "libwayland-egl.so.1",
              "libwayland-client.so.0", "libxkbcommon.so.0", "libXext.so.6", "libX11.so.6"]:
        dst = os.path.join(lib_dir, s)
        if not os.path.lexists(dst): os.symlink("libwine_headless_stubs.so", dst)

    # 7. Prefix & Mount stubs
    os.makedirs(os.path.join(ROOTFS, "host-rootfs"), exist_ok=True)
    os.chmod(os.path.join(ROOTFS, "host-rootfs"), 0o755)
    preload = os.path.join(ROOTFS, "etc/ld.so.preload")
    if not os.path.exists(preload): open(preload, "wb").close()
    os.chmod(preload, 0o644)

    wine_dir = os.path.join(ROOTFS, "root/.wine")
    dosdevices = os.path.join(wine_dir, "dosdevices")
    sys32 = os.path.join(wine_dir, "drive_c/windows/system32")
    os.makedirs(dosdevices, exist_ok=True)
    os.makedirs(sys32, exist_ok=True)
    c_link, z_link = os.path.join(dosdevices, "c:"), os.path.join(dosdevices, "z:")
    if not os.path.lexists(c_link): os.symlink("../drive_c", c_link)
    if not os.path.lexists(z_link): os.symlink("/", z_link)

    win_pe = os.path.join(ROOTFS, "usr/lib/wine/aarch64-windows")
    if os.path.exists(win_pe):
        for f in os.listdir(win_pe):
            dst = os.path.join(sys32, f)
            if not os.path.lexists(dst): os.symlink(f"/usr/lib/wine/aarch64-windows/{f}", dst)

    win32_pe = os.path.join(ROOTFS, "usr/lib/wine/i386-windows")
    if os.path.exists(win32_pe):
        syswow64 = os.path.join(wine_dir, "drive_c/windows/syswow64")
        os.makedirs(syswow64, exist_ok=True)
        for f in os.listdir(win32_pe):
            dst = os.path.join(syswow64, f)
            if not os.path.lexists(dst): os.symlink(f"/usr/lib/wine/i386-windows/{f}", dst)

run()
