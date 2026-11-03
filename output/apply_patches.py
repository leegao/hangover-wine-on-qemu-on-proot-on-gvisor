#!/usr/bin/env python3

import os

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


def apply_signature_patch(path, orig_sig, patch_sig, desc, expected_offset=None):
    """
    Applies a binary patch using unique contextual signatures.
    - First verifies the expected offset if provided.
    - If offset differs (due to recompilation or shifting), performs full binary scan.
    - Checks for the patched signature to ensure complete idempotency without false positives.
    """
    assert len(orig_sig) == len(patch_sig), f"Signature lengths mismatch for {desc}"
    if not os.path.exists(path):
        print(f"[SKIP] {path} not found for {desc}")
        return False

    with open(path, "rb") as f:
        data = bytearray(f.read())

    plen = len(orig_sig)

    if expected_offset is not None and expected_offset + plen <= len(data):
        if data[expected_offset : expected_offset + plen] == patch_sig:
            print(f"[OK] Already patched: {desc}")
            return True
        if data[expected_offset : expected_offset + plen] == orig_sig:
            data[expected_offset : expected_offset + plen] = patch_sig
            with open(path, "wb") as f:
                f.write(data)
            print(f"[PATCHED] {desc} (at primary offset 0x{expected_offset:X})")
            return True

    patch_idx = data.find(patch_sig)
    if patch_idx != -1:
        print(f"[OK] Already patched: {desc} (at dynamic offset 0x{patch_idx:X})")
        return True

    orig_idx = data.find(orig_sig)
    if orig_idx != -1:
        data[orig_idx : orig_idx + plen] = patch_sig
        with open(path, "wb") as f:
            f.write(data)
        print(f"[PATCHED] {desc} (at dynamic offset 0x{orig_idx:X})")
        return True

    raise RuntimeError(f"Signature not found in {os.path.basename(path)} for {desc}")


def run():
    # See NOTES.md for patch notes

    # 1. `/usr/bin/wine` executes:
    #       `/usr/lib/wine/aarch64-unix/wine-preloader /usr/lib/wine/aarch64-unix/wine hello.exe`.
    # 2. `wine-preloader` parses its own ELF headers and reserves low memory pages.
    # 3. Preloader opens `/lib/ld-linux-aarch64.so.1` and transfers control to its entry point.
    # 4. `ld-linux` inspects the initial stack for `AT_PHDR` and `AT_BASE`.
    # 5. `qemu-aarch64-static`'s (`AT_PHDR`, `AT_BASE`, `AT_ENTRY`) reflects `wine-preloader`, not `wine`.
    # 6. `ld-linux` dereferences an invalid base address, triggering an immediate fault
    for p in ["usr/bin/wine-preloader", "usr/lib/wine/aarch64-unix/wine-preloader"]:
        full = os.path.join(ROOTFS, p)
        if os.path.exists(full) and not os.path.islink(full):
            os.rename(full, full + ".bak")
            print(f"[PATCHED] Disabled preloader: {p} -> {p}.bak")

    ntdll = os.path.join(ROOTFS, "usr/lib/wine/aarch64-unix/ntdll.so")
    if os.path.exists(ntdll):
        # CPUID Emulation (wine direct patches, see below for qemu patches)
        # 1. Wine `ntdll.so` `cpu_init` executes `mrs x0, ID_AA64SMFR0_EL1` / `mrs x0, ID_AA64MMFR2_EL1` from EL0.
        # 2. QEMU 7.2.0 does not whitelist these ARMv8.2+ ID registers in `cp_reg_user_id_regs` -> flagged `PL1_R` (privileged only).
        # 3. QEMU raises `EXCP_UDEF` -> uncaught host `SIGILL` (Illegal instruction), terminating Wine.
        # Patch: Replace `mrs x0, ID_AA64*_EL1` with `mov x0, #0` so Wine reads 0 (feature unsupported) and proceeds.
        # mrs x0, ID_AA64SMFR0_EL1 -> mov x0, #0
        apply_signature_patch(
            ntdll,
            bytes.fromhex("a00438d5e40300aa"),
            bytes.fromhex("000080d2e40300aa"),
            "ntdll.so: mrs x0, ID_AA64SMFR0_EL1 -> mov x0, #0",
            expected_offset=0x4DFC0,
        )

        # mrs x0, ID_AA64MMFR2_EL1 -> mov x0, #0
        apply_signature_patch(
            ntdll,
            bytes.fromhex("400738d5e50300aa"),
            bytes.fromhex("000080d2e50300aa"),
            "ntdll.so: mrs x0, ID_AA64MMFR2_EL1 -> mov x0, #0",
            expected_offset=0x4E100,
        )


        # In wine `try_map_free_area` calls `mmap(flags | MAP_FIXED_NOREPLACE)`.
        # On gVisor this does not return `EEXIST` for occupied addresses, so Wine mistakes the result
        # for success and issues `MAP_FIXED` over active QEMU structures.
        # Patch: branch `try_map_free_area` to its fail handler on address mismatch (b 0x5ecc4)
        apply_signature_patch(
            ntdll,
            bytes.fromhex("c0035fd6000200f0147841f9"),
            bytes.fromhex("c0035fd61c000014147841f9"),
            "ntdll.so: try_map_free_area -> b 0x5ecc4",
            expected_offset=0x5EC50,
        )

        # Virtual Address Space Limits
        # 1. Wine reserves virtual address space up to `0x7fffffff0000`, colliding with QEMU's guest stack
        #    and binary mappings at `0x5500000000`–`0x5600000000`.
        # 2. QEMU's guest stack is unmapped and the emulator crashes with `SIGSEGV`.
        # Patch: clamp the low reserve start to `0x20000000`,
        #        clamp high reserve limits to `< 0x3fffffff0000`,
        #        clamp `.data` address/user/working_set limits
        #
        # low reserve start -> 0x20000000
        apply_signature_patch(
            ntdll,
            bytes.fromhex("0100add22000a0d267f3ff97"),
            bytes.fromhex("0100add20000a4d267f3ff97"),
            "ntdll.so: low reserve start -> 0x20000000",
            expected_offset=0x61F44,
        )

        # high reserve end & start
        apply_signature_patch(
            ntdll,
            bytes.fromhex("e17b70b2e05767b2"),
            bytes.fromhex("e17770b2e05367b2"),
            "ntdll.so: high reserve limits -> 0x3fffffff0000 / 0x3ffffe000000",
            expected_offset=0x61F5C,
        )

        # .data memory limits (address_space_limit, user_space_limit, working_set_limit)
        apply_signature_patch(
            ntdll,
            bytes.fromhex("0000ffffff7f00000000ffffff7f00000000ffffff7f0000"),
            bytes.fromhex("0000ffffff3f00000000ffffff3f00000000ffffff3f0000"),
            "ntdll.so .data: address/user/working_set limits",
            expected_offset=0x912F0,
        )

        # 1. x86-64 binary executes unaligned atomic `lock cmpxchg [rax+1], rbx`.
        # 2. FEX translates to AArch64 `ldaxr`/`stlxr` on an unaligned address -> Hardware Alignment Fault.
        # 3. Linux kernel raises `SIGBUS` (`si_signo = 7`).
        # 4. QEMU dispatches `TARGET_SIGBUS` without an `esr_context` frame (`ESR_MAGIC = 0x45535201` missing).
        # 5. Wine's `segv_handler` fails to find `ESR_MAGIC` and falls back to `STATUS_ACCESS_VIOLATION` (`0xc0000005`).
        # 6. Process crashes with unhandled memory access violation.
        # Patch: Short-circuit `segv_handler` at 0x3C530 to inspect `siginfo->si_signo` directly;
        #        on `SIGBUS`, assign `STATUS_DATATYPE_MISALIGNMENT` (`0x80000002`) so FEX's
        #        `HandleUnalignedAccess` backpatches the JIT block and execution resumes.
        #
        # SIGBUS -> STATUS_DATATYPE_MISALIGNMENT in segv_handler
        apply_signature_patch(
            ntdll,
            bytes.fromhex("7f0006eb230100540a000014650440b9"),
            bytes.fromhex("c40240b99f1c0071001e005409000014"),
            "ntdll.so: map SIGBUS directly to STATUS_DATATYPE_MISALIGNMENT",
            expected_offset=0x3C530,
        )

    # 1. Launching `wine hello.exe` invokes `wineboot.exe` to initialize the Wine prefix.
    # 2. `wineboot.exe` calls `create_hardware_registry_keys()` then continues into `start_services_process()`
    #    and `start_desktop()`.
    # 3. `services.exe` and `explorer.exe` fail to connect to X11/Wayland (headless) and `wineboot.exe`
    #    hangs indefinitely waiting for `__wine_boot_event`.
    # Patch: After registry key creation, branch directly to exit cleanup (`ExitProcess(0)`), completing
    #        boot initialization in ~0.2 seconds without launching GUI services.
    wb = os.path.join(ROOTFS, "usr/lib/wine/aarch64-windows/wineboot.exe")
    if os.path.exists(wb):
        apply_signature_patch(
            wb,
            bytes.fromhex("00013fd600e4006fe80000b0"),
            bytes.fromhex("00013fd6d3080014e80000b0"),
            "wineboot.exe: fast exit after registry init",
            expected_offset=0x12608,
        )

    # Heap Use-After-Free in libarm64ecfex.dll (TODO(leegao): fix in FEX)
    # 1. For executable paths >= 23 characters, `GetExecutableFilePath()` allocates a heap buffer via `rpmalloc`.
    # 2. In `ProcessInit`, `aligned_free` is called on the path buffer *before* `memmove` copies the basename.
    # 3. The free-list pointer overwrites the first character of the basename with `0x00`; `memmove` reads
    #    an empty string, corrupting the app name and triggering `ForcedAssert` (`brk #0x1`).
    # Patch: NOP the premature `aligned_free` call at 0x3064 so the buffer survives until after `memmove`.
    fex = os.path.join(ROOTFS, "usr/lib/wine/aarch64-windows/libarm64ecfex.dll")
    if os.path.exists(fex):
        apply_signature_patch(
            fex,
            bytes.fromhex("48000036f5700694b30219cb"),
            bytes.fromhex("480000361f2003d5b30219cb"),
            "libarm64ecfex.dll: NOP premature aligned_free (UAF fix)",
            expected_offset=0x3064,
        )

    win32u = os.path.join(ROOTFS, "usr/lib/wine/aarch64-unix/win32u.so")
    if os.path.exists(win32u):
        # `d3d11_bc7_sample.exe` creates a Win32 window before `D3D11CreateDeviceAndSwapChain`.
        #  1. Without X11/Wayland, Wine falls back to the `nodrv` driver; `CreateWindowExW` returns NULL.
        #  2. Even with the `null` graphics driver, `load_driver` (0x8A664) unconditionally overwrites
        #     `null_driver.pCreateWindow` (offset 0x468) with `nodrv_CreateWindow`.
        # Patch: in load_driver (0x8A664), NOP out `str x1, [x2, #0x468]` so null_driver keeps
        #        pCreateWindow = nulldrv_CreateWindow and returns a valid stub HWND
        apply_signature_patch(
            win32u,
            bytes.fromhex("e1fffff021802a91413402f900400591"),
            bytes.fromhex("e1fffff021802a911f2003d500400591"),
            "win32u.so: preserve null_driver.pCreateWindow in load_driver",
            expected_offset=0x8A664,
        )

        # `get_client_rect` (0x1885A0) returns a corrupted client rect (height 0) for CW_USEDEFAULT
        # windows, causing DXVK's presenter to skip swapchain creation/presentation.
        # Patch: Replace the client-rect validation epilogue in get_client_rect (0x1885A0) to populate
        #        `*rect = {0, 0, 800, 600}` and return TRUE.
        apply_signature_patch(
            win32u,
            bytes.fromhex(
                "620240b900008052610a40b95f00016baa000054600e40b9610640b93f00006be0a79f1a"
            ),
            bytes.fromhex(
                "02648052034b80527f7e0029620e0129200080521f2003d51f2003d51f2003d51f2003d5"
            ),
            "win32u.so: return 800x600 client_rect in headless get_client_rect",
            expected_offset=0x1885A0,
        )

    qemu_bin = os.path.join(PROOT_QEMU_DIR, "qemu-aarch64-static")
    if os.path.exists(qemu_bin):
        # 1. FEX allocates JIT code buffers with `PAGE_EXECUTE_READWRITE`; QEMU write-protects host pages
        #    to detect self-modifying code.
        # 2. On the first write, gVisor raises `SIGSEGV` (`SEGV_ACCERR`) but sets `REG_TRAPNO = 0`
        #    (instead of `0xe` for page faults).
        # 3. QEMU's `host_signal_handler` checks `REG_TRAPNO == 0xe` before `handle_sigsegv_accerr_write`;
        #    the check fails, QEMU forwards `SIGSEGV` to Wine, and Wine loops forever on the faulting instruction.
        # Patch: NOP the `cmp`/`je` sequence at 0x21F0E7 so every `SEGV_ACCERR` write fault branches directly
        #        to `handle_sigsegv_accerr_write`, which invalidates cached translations and repromotes
        #        the page to `PROT_WRITE`.
        apply_signature_patch(
            qemu_bin,
            bytes.fromhex("4585f60f84d0020000488b4c2418"),
            bytes.fromhex("4585f6909090909090488b4c2418"),
            "qemu-aarch64-static: bypass REG_TRAPNO for JIT writes",
            expected_offset=0x21F0E4,
        )

        # QEMU CPUID Emulation (qemu side)
        # 1. Modern AArch64 binaries query CPU features via `mrs` on ID registers (e.g. `ID_AA64SMFR0_EL1`).
        # 2. QEMU 7.2.0's `cp_reg_user_id_regs` only whitelists restrictive patterns like
        #    `ID_AA64ISAR*_EL1_RESERVED`, leaving ARMv8.2+ ID registers flagged `PL1_R`.
        # 3. Guest EL0 execution of `mrs` on these registers raises `EXCP_UDEF` -> fatal `SIGILL`.
        # Patch: Replace the whitelist pattern string with `ID_AA64*\x00` (universal prefix wildcard) so any
        #        `ID_AA64` system register is matched, marked `PL0_R`, and reads return 0.
        orig_qemu = b"ID_AA64ISAR*_EL1_RESERVED\x00"
        patch_qemu = b"ID_AA64*\x00" + b"\x00" * (len(orig_qemu) - len(b"ID_AA64*\x00"))
        apply_signature_patch(
            qemu_bin,
            orig_qemu,
            patch_qemu,
            "qemu-aarch64-static: universal ID_AA64* CPUID user-mode read whitelist",
            expected_offset=0x4660E9,
        )


if __name__ == "__main__":
    run()
