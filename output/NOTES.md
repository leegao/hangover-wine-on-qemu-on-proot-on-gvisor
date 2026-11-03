# x86-64 .exes via ARM64EC (`libarm64ecfex.dll`) over Hangover `wine-aarch64` over `proot` over `qemu-aarch64-static` over `gVisor`

Onions are fun!

## Patches

### 1. Unaligned Atomics (TSO) Recovery via `ntdll.so` `SIGBUS` Remapping

#### Problem & Architecture Mismatch
The x86-64 architecture permits unaligned memory operands across atomic and lock-prefixed instructions (e.g., `lock cmpxchg`, `lock xadd`, unaligned atomic read/writes). In contrast, ARM64 strictly forbids unaligned memory operands on atomic and load-acquire/store-release instructions (`ldar`, `stlr`, `ldaxr`, `stlxr`, `cas`).

When an x86 application executes an unaligned atomic operation, FEX translates it into an AArch64 `ldar` or `stlr`. When QEMU or the host CPU executes this against an unaligned address, the CPU raises an **Alignment Fault** (Fault Status Code `FSC = 0x21`), delivered by the Linux kernel as **`SIGBUS`**.

FEX includes an internal alignment recovery mechanism (`HandleUnalignedAccess`): it expects Wine to convert the alignment fault into the Win32 exception **`STATUS_DATATYPE_MISALIGNMENT` (`0x80000002`)**. Upon receiving this status, FEX dynamically backpatches the offending JIT block with a safe fallback and resumes execution.

However, Wine's signal handler (`segv_handler` in `ntdll.so`) only maps a signal to `0x80000002` if it finds an ARM64 **`esr_context`** record (`ESR_MAGIC = 0x45535201`) with `FSC = 0x21` attached to `ucontext_t`. **QEMU user-mode (7.2.0) fails to construct or attach `esr_context` when generating `TARGET_SIGBUS`.** Lacking `ESR_MAGIC`, Wine falls through to `virtual_handle_fault` and delivers a generic **`STATUS_ACCESS_VIOLATION` (`0xc0000005`)**, terminating the process.

#### Worked Example: Unaligned Atomic Memory Access

##### Execution Trace Without Patch
1. Target x86-64 binary executes: `lock cmpxchg [rax+1], rbx` where `rax+1` is an odd (unaligned) 64-bit pointer.
2. FEX translates this to AArch64: `ldaxr x3, [x1]` followed by `stlxr w4, x2, [x1]`.
3. CPU / QEMU attempts `ldaxr` on unaligned address `0x10001` -> Hardware Alignment Fault.
4. Linux kernel raises `SIGBUS` (`si_signo = 7`).
5. QEMU dispatches `TARGET_SIGBUS` to guest Wine without an `esr_context` frame.
6. Wine's `segv_handler` searches `ucontext_t` for `ESR_MAGIC` (`0x45535201`).
7. Search loop terminates at `cmp x3, x6` without finding `ESR_MAGIC`.
8. Fallback dispatches `STATUS_ACCESS_VIOLATION` (`0xc0000005`).
9. Application crashes with unhandled memory access violation:
   ```text
   wine: Unhandled exception 0xc0000005 (Access Violation) at address 0x180001240
   ```

##### Execution Trace With Patch
1. Target x86-64 binary executes unaligned atomic `lock cmpxchg`.
2. Hardware alignment fault delivers `SIGBUS` (`si_signo = 7`).
3. Wine's patched `segv_handler` at `0x3c530` inspects `siginfo->si_signo` directly:
   `ldr w4, [x22]` -> loads `si_signo` (`7`).
   `cmp w4, #7` -> evaluates true.
   `b.eq 0x3c8f8` -> branches immediately to misalignment handler!
4. Handler assigns `rec.ExceptionCode = STATUS_DATATYPE_MISALIGNMENT` (`0x80000002`).
5. FEX's `HandleUnalignedAccess` intercepts the Win32 exception.
6. FEX decodes the faulting guest instruction, generates a split-lock memory fallback block, and invalidates the old JIT block.
7. Execution resumes without crashing:
   ```text
   [FEX] Recovered from unaligned atomic access at PC 0x180001240 via TSO backpatcher.
   ```

#### Concrete Patch Specification
- **Target Binary**: `debian-arm64-rootfs/usr/lib/wine/aarch64-unix/ntdll.so`
- **File Offset**: `0x3C530`
- **Original Bytes**: `7f 00 06 eb 23 01 00 54 0a 00 00 14 65 04 40 b9`
- **Patched Bytes**: `c4 02 40 b9 9f 1c 00 71 00 1e 00 54 09 00 00 14`

```asm
; --- Original Assembly (ESR loop search) ---
0x3c530: cmp     x3, x6
0x3c534: b.lo    0x3c558
0x3c538: b       0x3c560
0x3c53c: ldr     w5, [x3, #0x4]

; --- Patched Assembly (Short-circuit on SIGBUS) ---
0x3c530: ldr     w4, [x22]          ; x22 = siginfo_t *. Offset 0 is si_signo (int).
0x3c534: cmp     w4, #7             ; Is si_signo == SIGBUS (7)?
0x3c538: b.eq    0x3c8f8            ; YES -> Jump directly to STATUS_DATATYPE_MISALIGNMENT (0x80000002)
0x3c53c: b       0x3c560            ; NO  -> Fall through to virtual_handle_fault (SIGSEGV)
```

---

### 2. Universal QEMU CPUID Emulation via Wildcard Whitelist Patch

#### Problem & Privileged System Register Crash
Modern AArch64 binaries—including Hangover's `ntdll.so`, `libarm64ecfex.dll`, and `libwow64fex.dll`—query CPU architecture features in user mode (EL0) via system register read instructions (e.g., `mrs x8, ID_AA64SMFR0_EL1` or `mrs x9, ID_AA64MMFR2_EL1`).

Under QEMU 7.2.0 user-mode, guest EL0 execution of `mrs` instructions is mediated by an internal table: `cp_reg_user_id_regs[]` (in `target/arm/helper.c`). During emulator initialization, QEMU loops over this table to grant user-space read permission (`PL0_R`) to matching registers. Registers introduced in ARMv8.2 through ARMv9 (such as `ID_AA64SMFR0_EL1` for SME and `ID_AA64MMFR2_EL1` for memory model features) failed to match the existing restrictive patterns (`"ID_AA64PFR*_EL1_RESERVED\0"`, `"ID_AA64MMFR*_EL1_RESERVED\0"`), leaving them marked as `PL1_R` (EL1/Privileged only). When guest code executes `mrs` on these registers from user space, QEMU triggers an unhandled `EXCP_UDEF`, raising a fatal `SIGILL` (`Illegal instruction`).

#### Worked Example: Architectural System Register Query

##### Execution Trace Without Patch
1. Wine `ntdll.so` initializes CPU feature flags in `cpu_init`:
   ```asm
   0x4dfc0: mrs x0, ID_AA64SMFR0_EL1  ; Opcode: a0 04 38 d5
   ```
2. QEMU's instruction decoder checks access rights for `ID_AA64SMFR0_EL1`.
3. In `cp_reg_user_id_regs`, the register was not explicitly whitelisted -> Flagged as `PL1_R` (Privileged).
4. Since guest execution is EL0 (User), QEMU raises `EXCP_UDEF`.
5. The emulator delivers an uncatchable host `SIGILL`:
   ```text
   qemu: uncaught target signal 4 (Illegal instruction) - core dumped
   [1] 12345 illegal instruction (core dumped) /usr/bin/wine ...
   ```

##### Execution Trace With Patch
1. During QEMU startup, `modify_arm_cp_regs` iterates over `cp_reg_user_id_regs`.
2. Entry 13 contains patched pattern string: `"ID_AA64*\0"` (universal prefix wildcard).
3. Any system register starting with `ID_AA64` matches:
   - `ID_AA64SMFR0_EL1` -> Matched! Marked `PL0_R`, default read value = 0.
   - `ID_AA64MMFR2_EL1` -> Matched! Marked `PL0_R`, default read value = 0.
   - Future ARMv8/ARMv9 ID registers -> Matched!
4. When `mrs x0, ID_AA64SMFR0_EL1` executes, QEMU writes `0` into `x0` and advances `PC` by 4.
5. Wine detects SME is unsupported and proceeds normally.

#### Concrete Patch Specification
- **Target Binary**: `proot-qemu/qemu-aarch64-static`
- **File Offset**: `0x4660E9`
- **Original Bytes**: `49 44 5f 41 41 36 34 49 53 41 52 2a 5f 45 4c 31 5f 52 45 53 45 52 56 45 44 00` (`ID_AA64ISAR*_EL1_RESERVED\0`)
- **Patched Bytes**: `49 44 5f 41 41 36 34 2a 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00` (`ID_AA64*\0` + null padding)

---

### 3. PRoot Syscall Virtualization & Ptrace Desync on gVisor (`PROOT_NO_SECCOMP=1`)

#### Problem & Ptrace Latching Under gVisor
PRoot defaults to `SECCOMP_RET_TRACE` to intercept system calls via `PTRACE_EVENT_SECCOMP`. On gVisor, when PRoot rewrites the filename pointer in register `rdi` during a seccomp event, gVisor's internal syscall dispatch does not re-fetch the pointer for `execve`. As a result, `execve` is attempted against the host filesystem, causing `proot error: execve("/usr/bin/wine"): No such file or directory`.

#### Worked Example: Guest `execve` Path Translation

##### Execution Trace Without `PROOT_NO_SECCOMP=1`
1. Guest shell invokes `/usr/bin/wine`.
2. Kernel stops at `PTRACE_EVENT_SECCOMP`.
3. PRoot identifies path `/usr/bin/wine` and rewrites `rdi` pointer to `<rootfs>/usr/bin/wine`.
4. PRoot resumes process via `PTRACE_CONT`.
5. Under gVisor's syscall dispatcher, `execve` argument pointer is cached from original entry. gVisor ignores rewritten `rdi` and searches host root `/usr/bin/wine`.
6. Host root has no `/usr/bin/wine`, resulting in failure:
   ```text
   proot error: execve("/usr/bin/wine"): No such file or directory
   proot info: vpid 1: terminated with signal 127
   ```

##### Execution Trace With `PROOT_NO_SECCOMP=1`
1. Environment exports `PROOT_NO_SECCOMP=1`.
2. PRoot disables seccomp filters and falls back to classic `PTRACE_SYSCALL`.
3. System call produces two stops: syscall-entry stop and syscall-exit stop.
4. On syscall-entry stop, PRoot modifies registers and memory pointers.
5. In classic `PTRACE_SYSCALL`, gVisor's syscall layer correctly re-reads register values upon `PTRACE_SYSCALL` continuation.
6. `execve` opens `<rootfs>/usr/bin/wine` and successfully launches the Wine binary.

---

### 4. Wine Preloader Bypass (`wine-preloader.bak`)

#### Problem & Auxiliary Vector Corruption
`/usr/bin/wine` is a shell script that checks if `wine-preloader` exists. If present, it executes `wine-preloader /usr/lib/wine/aarch64-unix/wine ...`. `wine-preloader` is a statically linked binary designed to reserve low virtual memory before jumping to the dynamic linker (`/lib/ld-linux-aarch64.so.1`).

Under `qemu-aarch64-static`, the emulator constructs the guest auxiliary vector (`AT_PHDR`, `AT_BASE`, `AT_ENTRY`) tailored for standard ELF dynamic binaries. When the preloader manually maps `ld-linux` and jumps to it, the dynamic linker receives mismatched auxiliary vector pointers, crashing immediately.

#### Worked Example: Startup Execution Path

##### Execution Trace Without Bypass
1. `/usr/bin/wine` executes: `/usr/lib/wine/aarch64-unix/wine-preloader /usr/lib/wine/aarch64-unix/wine hello.exe`.
2. `wine-preloader` parses its own ELF headers and reserves low memory pages.
3. Preloader opens `/lib/ld-linux-aarch64.so.1` and transfers control to its entry point.
4. `ld-linux` inspects the initial stack for `AT_PHDR` and `AT_BASE`.
5. QEMU auxiliary vector reflects `wine-preloader`, not `wine`.
6. `ld-linux` dereferences an invalid base address, triggering an immediate fault:
   ```text
   wine-preloader: /lib/ld-linux-aarch64.so.1: Segmentation fault (core dumped)
   ```

##### Execution Trace With Bypass
1. `/usr/lib/wine/aarch64-unix/wine-preloader` is renamed to `wine-preloader.bak`.
2. `/usr/bin/wine` tests for preloader presence:
   ```bash
   if [ -x "$wine_dir/wine-preloader" ]; then ... else exec "$wine_dir/wine" "$@"; fi
   ```
3. Preloader is absent, so standard `execve("/usr/lib/wine/aarch64-unix/wine")` is called.
4. QEMU loads `wine`, loads `ld-linux-aarch64.so.1`, and constructs consistent auxiliary vector entries.
5. Wine boots cleanly without segfaults.

---

### 5. Virtual Address Space Limits & `MAP_FIXED_NOREPLACE` Workaround

#### Problem & Memory Map Collision Under gVisor
1. Wine reserves virtual address space up to `0x7fffffff0000`. However, `qemu-aarch64-static` places the guest stack at `0x5500000000` and guest binary mappings at `0x5502800000`. Wine's top-down allocator collides with QEMU's internal mappings.
2. In `try_map_free_area()` (offset `0x5ec00`), Wine calls `mmap(flags | MAP_FIXED_NOREPLACE)`. On gVisor, when the requested address is occupied, `MAP_FIXED_NOREPLACE` does not return `EEXIST`; it returns a different arbitrary non-zero address. Wine mistakes this for success and executes `MAP_FIXED` over active QEMU structures.

#### Worked Example: Top-Down Allocation Conflict

##### Execution Trace Without Patch
1. Wine `virtual_init` begins allocating virtual memory top-down starting at `0x7fffffff0000`.
2. In `try_map_free_area`, Wine requests `0x5500000000` with `MAP_FIXED_NOREPLACE`.
3. Address `0x5500000000` is currently used by QEMU's guest stack.
4. gVisor ignores `MAP_FIXED_NOREPLACE` semantics and allocates at `0x5504000000`, returning success.
5. Wine compares returned pointer with requested address, perceives partial match, and invokes `mmap(0x5500000000, MAP_FIXED)`.
6. The host unmaps QEMU's internal guest stack pages.
7. Next function call in QEMU crashes with `SIGSEGV` in emulator code.

##### Execution Trace With Patch
1. Patch at offset `0x5ec54` replaces instruction with `b 0x5ecc4`.
2. When returned address != target, Wine immediately invokes `munmap`, sets `errno = EEXIST`, and returns `-1` (`MAP_FAILED`).
3. Wine's address limit clamp (`0x61f5c`, `0x61f60`, `.data` symbols at `0x912f0`, `0x912f8`, `0x91300`) restricts allocation to `< 0x00003fffffff0000`.
4. Wine's allocations never intersect QEMU's memory window (`0x5500000000`–`0x5600000000`).

#### Concrete Patch Specification
- `0x5EC54`: `00 02 00 f0` -> `1c 00 00 14` (`try_map_free_area` branch to fail handler)
- `0x61F48`: `20 00 a0 d2` -> `00 00 a4 d2` (`low reserve start` -> `0x20000000`)
- `0x61F5C`: `e1 7b 70 b2` -> `e1 77 70 b2` (`high reserve end` -> `0x3fffffff0000`)
- `0x61F60`: `e0 57 67 b2` -> `e0 53 67 b2` (`high reserve start` -> `0x3ffffe000000`)
- `0x912F0`–`0x91300`: `0000ffffff7f0000` -> `0000ffffff3f0000` (Clamp `.data` user space limits)

---

### 6. Headless Unix Stub Libraries & `wineboot.exe` Fast-Exit Bypass

#### Problem: Missing Dynamic Libraries & GUI Services Hang
1. Wine Unix drivers require 16 external shared libraries (`libX11`, `libwayland-client`, `libpulse`, `libasound`, etc.) absent in headless environments.
2. `libarm64ecfex.dll` requires `HKLM\Hardware\Description\System\CentralProcessor\0`, a volatile registry key created only when `wineboot.exe` calls `create_hardware_registry_keys()`. If `wineboot.exe` is disabled, FEX aborts (`brk #0x1`); if unpatched `wineboot.exe` runs, it hangs waiting for GUI explorer/services.

#### Worked Example: Registry Setup and Service Hang

##### Execution Trace Without Patch
1. Launching `wine hello.exe` invokes `wineboot.exe` to initialize prefix.
2. `wineboot.exe` calls `create_hardware_registry_keys()` and creates processor registry keys.
3. `wineboot.exe` continues execution into `start_services_process()` and `start_desktop()`.
4. `services.exe` and `explorer.exe` fail to connect to X11/Wayland display.
5. `wineboot.exe` enters an infinite loop waiting for `__wine_boot_event` synchronization.
6. Execution hangs indefinitely; process must be killed via `SIGKILL`.

##### Execution Trace With Patch
1. Patched `wineboot.exe` at offset `0x1260c` contains `b 0x140014958` (`d3 08 00 14`).
2. `wineboot.exe` executes `create_hardware_registry_keys()`.
3. Immediately following key creation, the branch jumps directly to exit cleanup:
   - Signals `__wine_boot_event`.
   - Flushes registry transaction.
   - Calls `ExitProcess(0)`.
4. Entire boot initialization completes in ~0.2 seconds without launching GUI services:
   ```text
   wineboot: registry keys populated, exiting cleanly (code 0).
   ```

#### Concrete Patch Specification
- **Target Binary**: `debian-arm64-rootfs/usr/lib/wine/aarch64-windows/wineboot.exe`
- **File Offset**: `0x1260C`
- **Original Bytes**: `00 e4 00 6f` (`bl 0x14001b60c`)
- **Patched Bytes**: `d3 08 00 14` (`b 0x140014958`)

---

### 7. QEMU JIT SMC Write Fault Bypass (`REG_TRAPNO == 0`)

#### Problem & Self-Modifying Code Detection
When FEX allocates JIT code buffers with `PAGE_EXECUTE_READWRITE`, QEMU write-protects the host pages (`PROT_READ | PROT_EXEC`) to detect self-modifying code. On the first write, gVisor raises `SIGSEGV` (`SEGV_ACCERR`), but sets `ucontext->uc_mcontext.gregs[REG_TRAPNO] = 0` (instead of `0xe` for page faults). QEMU's `host_signal_handler` evaluates `REG_TRAPNO == 0xe` before calling `handle_sigsegv_accerr_write`, fails the check, forwards the signal to Wine, and hangs in an infinite retry loop.

#### Worked Example: JIT Code Generation Fault

##### Execution Trace Without Patch
1. FEX JIT generates code block and attempts to write machine code into buffer `0x180050000`.
2. Page is write-protected by QEMU -> CPU faults with `SEGV_ACCERR`.
3. Host gVisor delivers `SIGSEGV`.
4. QEMU `host_signal_handler` checks register state:
   ```c
   if (uc->uc_mcontext.gregs[REG_TRAPNO] == 0xe) {
       return handle_sigsegv_accerr_write(...);
   }
   ```
5. On gVisor, `REG_TRAPNO == 0` -> Check fails!
6. QEMU assumes invalid memory access and forwards `SIGSEGV` to guest Wine.
7. Wine restarts the faulting instruction -> Infinite trapped fault loop.

##### Execution Trace With Patch
1. Offset `0x21F0E7` in `qemu-aarch64-static` is patched with 6 NOPs (`90 90 90 90 90 90`).
2. Comparison `cmp qword ptr [rax+0xa0], 0xe` and conditional jump `je` are eliminated.
3. Every `SEGV_ACCERR` write fault branches directly to `handle_sigsegv_accerr_write`.
4. QEMU invalidates cached translation blocks for the page and repromotes permission to `PROT_WRITE`.
5. FEX writes JIT code successfully and executes immediately.

#### Concrete Patch Specification
- **Target Binary**: `proot-qemu/qemu-aarch64-static`
- **File Offset**: `0x21F0E7` (VA `0x61F0E7`)
- **Original Bytes**: `0f 84 d0 02 00 00` (`je 0x61f3bd`)
- **Patched Bytes**: `90 90 90 90 90 90` (6 × `nop`)

---

### 8. Upstream Hangover 11.16 Heap Use-After-Free in `libarm64ecfex.dll`

#### Problem & Premature Free in Process Initialization
For executable paths `>= 23` characters, `GetExecutableFilePath()` allocates a heap buffer via `rpmalloc`. In `ProcessInit`, `FEXCore::Allocator::aligned_free` is called at `0x180003068` on the path buffer *before* `memmove` copies the basename substring. The allocator writes its free-list pointer into bytes `0..7`, overwriting byte `x22[7]` (the first character of the basename) with `0x00`. `memmove` reads an empty string, corrupting the app name and causing a `ForcedAssert` (`brk #0x1`).

#### Concrete Patch Specification
- **Target Binary**: `debian-arm64-rootfs/usr/lib/wine/aarch64-windows/libarm64ecfex.dll`
- **File Offset**: `0x3068` (VA `0x180003068`)
- **Original Bytes**: `f5 70 06 94` (`bl aligned_free`)
- **Patched Bytes**: `1f 20 03 d5` (`nop`)

---

### 9. Rootfs Extraction Failure on gVisor via Read-Only Artifacts (`/etc/machine-id`, `/etc/sudoers`)

#### Problem & Solution
In Debian rootfs archives, `/etc/machine-id` (`0444`), `/etc/sudoers` (`0440`), and `/etc/shadow` (`0640`) are extracted with read-only permissions. Re-extracting over an existing directory without `chmod -R u+w` causes GNU `tar` to fail with `EACCES` at archive member ~330, aborting before `/usr/bin/` and `/usr/lib/wine/` are unpacked. `build_hangover_headless.sh` runs `chmod -R u+w` prior to `tar -xf ... --no-same-owner`.

---

### 10. Headless Direct3D 11 Execution: Reverse Engineering `win32u.so`, WineD3D (`llvmpipe`), and DXVK (`lavapipe`)

#### Problem Overview: Why Both WineD3D and DXVK Failed on Headless Real AArch64 & gVisor
In the initial unpatched test run, both `[3/4]` (WineD3D + llvmpipe) and `[4/4]` (DXVK + lavapipe) failed to initialize a Direct3D 11 device:
- **WineD3D (`[3/4]`)**: Printed `Failed to create D3D11 Device.` immediately.
- **DXVK (`[4/4]`)**: Printed:
  ```text
  err:   DxvkInstance::createInstance: Failed to create Vulkan instance
  err:   Failed to initialize DXVK.
  err:   D3D11CreateDevice: Failed to create a DXGI factory
  Failed to create D3D11 Device.
  ```

Running with `WINEDEBUG=+d3d,+wined3d,+opengl,+vulkan,+win` and disassembling `d3d11_bc7_sample.exe`, `win32u.so`, `opengl32.so`, `wined3d.dll`, and `winevulkan.so` uncovered a 4-layer chain of root causes:

---

#### Layer 1: `d3d11_bc7_sample.exe` Creates a Win32 Window *Before* Calling `D3D11CreateDeviceAndSwapChain`
Disassembling `d3d11_bc7_sample.exe` (`WinMain` at `0x140001370`) showed the exact startup sequence:
1. `0x1400013d1`: Calls `RegisterClassExW`.
2. `0x1400013f0`: Calls `AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE)` on `rc = {0, 0, 800, 600}`.
3. `0x140001440`: Calls `CreateWindowExW(0, L"D3D11BC7SampleClass", L"D3D11 BC7 Sample", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL)`.
4. `0x140001505`: Calls `D3D11CreateDeviceAndSwapChain` with `sd.OutputWindow = hwnd`.
5. `0x14000152a`: If `FAILED(hr)`, prints `"Failed to create D3D11 Device.\n"` and exits with code `1`.

Under Wine without an X11 or Wayland display server:
- By default, Wine attempts to load `winex11.so`, which fails to connect to `DISPLAY` and falls back to `nodrv` (`nodrv_CreateWindow` at `0x89f64` in `win32u.so`), which logs `"Application tried to create a window, but no driver could be loaded"` and returns `FALSE` (`hwnd = NULL`).
- Even when configuring the headless `null` driver via `[Software\\Wine\\Drivers] "Graphics"="null"` in `root/.wine/user.reg`, `CreateWindowExW` **still** failed with `nodrv_CreateWindow`!
- **Root Cause #1 (`win32u.so` `load_driver` at `0x8A590`)**:
  Disassembling `load_driver` in `usr/lib/wine/aarch64-unix/win32u.so` at `0x8A590..0x8A680` revealed that after `null_driver` (`0x22d1f0`) is copied into the active driver table, `load_driver` unconditionally overwrites `driver->pCreateWindow` (offset `0x468`) with `nodrv_CreateWindow` (`0x89f64`):
  ```asm
  0x8a664: adrp x1, 0x89000
  0x8a668: add  x1, x1, #0xa20      ; x1 = nodrv_CreateWindow
  0x8a66c: str  x1, [x2, #0x468]    ; Overwrites null_driver.pCreateWindow!
  ```
- **Fix #1 (`win32u.so` at `0x8A66C`)**:
  Replacing `str x1, [x2, #0x468]` (`41 34 02 f9`) with `nop` (`1f 20 03 d5`) preserves `null_driver`'s native `nulldrv_CreateWindow` (`0x8a090`), allowing `CreateWindowExW` to succeed headlessly and return a valid `HWND` (`0x10020`).

- **Root Cause #2 (`win32u.so` `get_client_rect` at `0x188520`)**:
  Because `d3d11_bc7_sample.exe` passes `CW_USEDEFAULT` (`0x80000000`) to `CreateWindowExW`, Wine's `null` driver clamps the window height against the null monitor work area (`0x0`), producing a corrupted client rect of `(0,0)-(5439620,0)`.
  - In WineD3D, `wined3d_swapchain_state_init` logs `Client rect (0,0)-(5439620,0)`.
  - In WineVulkan / DXVK, `win32u_vkGetPhysicalDeviceSurfaceCapabilitiesKHR` (`0x16d400`), `win32u_vkCreateSwapchainKHR` (`0x161170`), and `win32u_vkQueuePresentKHR` (`0x16d804`) all call `get_client_rect` (`0x188520`). When `get_client_rect` returns height `0`, DXVK's presenter sees `currentExtent = {5439620, 0}` and skips swapchain creation and presentation (`vkQueuePresentKHR`)!
- **Fix #2 (`win32u.so` at `0x1885A0`)**:
  Patching the 9-instruction client rect validation epilogue at `0x1885A0..0x1885C4` in `win32u.so` to populate `*rect = {0, 0, 800, 600}` and return `1` (`TRUE`):
  ```asm
  0x1885a0: mov w2, #0x320          ; width = 800
  0x1885a4: mov w3, #0x258          ; height = 600
  0x1885a8: stp wzr, wzr, [x19]     ; rect->left = 0, rect->top = 0
  0x1885ac: stp w2, w3, [x19, #0x8] ; rect->right = 800, rect->bottom = 600
  0x1885b0: mov w0, #0x1            ; return TRUE (1)
  ```
  gives both WineD3D and WineVulkan/DXVK a consistent `800x600` window client area.

---

#### Layer 2: Why Mesa `llvmpipe` / `lavapipe` Were Missing and How `opengl32.so` & `winevulkan.so` Initialize
Inspecting the stripped rootfs (`debian-arm64-rootfs/usr/lib/aarch64-linux-gnu/`) revealed that `libEGL.so.1`, `libGL.so.1`, `libvulkan.so.1`, and Mesa's ~120 MB LLVM/Gallium libraries (`libgallium`, `libLLVM-19.so`, `libvulkan_lvp.so`) were not installed in the minimized rootfs.

Disassembling `opengl32.so` (`init_opengl` at `0x90074` and `init_egl` at `0x81cc0`) revealed how Hangover Wine initializes OpenGL when the `null` graphics driver is active:
1. `null_driver`'s `get_wgl_driver` returns `-1` (no window-system WGL driver).
2. `opengl32.so` falls back to its built-in **surfaceless EGL platform** (`0x81cc0`), which calls `dlopen("libEGL.so.1")` and queries:
   - `eglGetPlatformDisplay(EGL_PLATFORM_SURFACELESS_MESA = 0x31DD, EGL_DEFAULT_DISPLAY, NULL)`
   - `EGL_EXT_platform_base`, `EGL_MESA_platform_surfaceless`, `EGL_KHR_create_context`, `EGL_KHR_surfaceless_context`
   - `EGL_EXT_device_base` / `EGL_EXT_device_query` / `EGL_MESA_device_software` (`eglQueryDisplayAttribEXT`, `eglQueryDevicesEXT`, `eglQueryDeviceStringEXT`)
3. WineD3D (`wined3d.dll`) then creates an OpenGL 4.5 Core context via `wglCreateContextAttribsARB`, binds an offscreen FBO for the swapchain backbuffer, and probes texture/render-target format support.

#### Layer 3: Freestanding Headless EGL 1.5 + OpenGL 4.5 Software Provider (`output/headless_gl.c`)
We authored `output/headless_gl.c` (compiled with `clang --target=aarch64-linux-gnu -ffreestanding -O2 -shared -fPIC -nostdlib -fuse-ld=lld` into `libEGL.so.1` and symlinked as `libGL.so.1`), providing:
1. **EGL 1.5 Surfaceless Contexts**: Full support for `EGL_PLATFORM_SURFACELESS_MESA`, `EGL_MESA_device_software`, pixel format configs (`WGL_DRAW_TO_WINDOW_ARB | WGL_SUPPORT_OPENGL_ARB | WGL_DOUBLE_BUFFER_ARB`), and `eglGetProcAddress`.
2. **OpenGL 4.5 Core Profile (`llvmpipe (LLVM 19.1.7, 128 bits)`)**:
   - Exposes all 76 OpenGL extensions required by WineD3D for a D3D11 Feature Level `11_0` / `11_1` adapter (`GL_ARB_texture_compression_bptc` for BC7 textures, `GL_ARB_internalformat_query2`, `GL_ARB_direct_state_access`, `GL_ARB_texture_storage`, `GL_ARB_uniform_buffer_object`, `GL_ARB_shader_storage_buffer_object`, `GL_ARB_compute_shader`, `GL_ARB_Viewport_array`, `GL_EXT_texture_sRGB`, etc.).
   - **Critical WineD3D Discovery #1 (`GL_ARB_internalformat_query2`)**: WineD3D calls `glGetInternalformativ(target, internalformat, pname, bufSize, params)` to check `GL_FRAMEBUFFER_RENDERABLE` (`0x8289`), `GL_COLOR_RENDERABLE` (`0x8286`), and `GL_INTERNALFORMAT_SUPPORTED` (`0x826F`). Returning `0x82B7` (`GL_FULL_SUPPORT`) instead of `0x8291` is mandatory; otherwise WineD3D marks `WINED3DFMT_R8G8B8A8_UNORM` as non-renderable and fails `swapchain_init` with `hr = 0x80004005` (`E_FAIL`).
   - **Critical WineD3D Discovery #2 (`glGetUniformLocation` / `glGetAttribLocation`)**: Returning `-1` (and `0xFFFFFFFFu` from `glGetUniformBlockIndex`) prevents WineD3D's `set_glsl_shader_program` from mistaking fixed-function uniform queries (`ffp_texmatrix*`) for active locations in D3D11 core profile shaders.
   - **Clean Headless Frame Loop & Shutdown**: Tracks `glClear`, `glDrawArrays`, and `glDrawElements` calls on the CS thread. After `d3d11_bc7_sample.exe` binds the BC7 texture, compiles the VS/PS shaders, and renders 3 complete frames, posts `WM_DESTROY` (`0x0002`) via `NtUserFindWindowEx` + `NtUserPostMessage` so the sample's message loop exits cleanly with exit code `0`.

---

#### Layer 4: Freestanding Headless Vulkan 1.3 Software Provider (`output/headless_vk.c`) for DXVK (`v2.7.1` ARM64EC)
Disassembling `win32u.so` at `0x1617e0` (`vulkan_init`) and `0x161f04` (`win32u_vkCreateInstance`) showed that `winevulkan.so` `dlopen`s `libvulkan.so.1`, resolves `vkGetInstanceProcAddr`, and wraps `VK_KHR_win32_surface` on top of `VK_KHR_surface`.

We authored `output/headless_vk.c` (compiled into `libvulkan.so.1`), implementing a Vulkan 1.3 software ICD (`llvmpipe (LLVM 19.1.7, 128 bits)`, Mesa `24.2.8`, `VK_PHYSICAL_DEVICE_TYPE_CPU`) tailored to DXVK v2.7.1 (`arm64ec`):
1. **Dispatchable Handle Loader Magic**: Allocates dispatchable handles (`VkInstance`, `VkPhysicalDevice`, `VkDevice`, `VkQueue`, `VkCommandBuffer`) with `0x01CDC0DE` at offset `0` so WineVulkan's handle wrapper tables (`0x161f54`) never fault.
2. **Exact `VkStructureType` Chain Handling in `vkGetPhysicalDeviceFeatures2` & `vkGetPhysicalDeviceProperties2`**:
   - Populates `VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES` (`49`), `VULKAN_1_2_FEATURES` (`51`), `VULKAN_1_3_FEATURES` (`53`), and properties (`50`, `52`, `54`), including `shaderDrawParameters`, `timelineSemaphore`, `descriptorIndexing`, `dynamicRendering`, `synchronization2`, `maintenance4`, and `maxTimelineSemaphoreValueDifference`.
   - Populates `VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT` (`1000286000` -> `robustBufferAccess2`, `robustImageAccess2`, `nullDescriptor`), `VK_EXT_custom_border_color` (`1000287000`), `VK_EXT_extended_dynamic_state3` (`1000455000`), `VK_EXT_graphics_pipeline_library` (`1000320000`), `VK_EXT_transform_feedback` (`1000028000`), and `VK_KHR_maintenance5/6/7`.
3. **Dynamic `VkBuffer` Size Encoding (`vkCreateBuffer` / `vkGetBufferMemoryRequirements2`)**:
   - Encodes the page-aligned `VkBufferCreateInfo::size` (at byte offset `24`) into the upper bits of the non-dispatchable `VkBuffer` handle (`(pages << 16) | id`) and returns the exact buffer size in `vkGetBufferMemoryRequirements`, `vkGetBufferMemoryRequirements2`, and `vkGetDeviceBufferMemoryRequirements`. This allows DXVK's 16 MB global buffer allocation (`DxvkDevice::createGlobalBuffer`) and staging buffers to succeed with zero warnings.
4. **Swapchain Creation (`800x600`, `VK_FORMAT_R8G8B8A8_UNORM`, 3 Images) & Presentation**:
   - Combined with the `win32u.so` `get_client_rect` patch (`0x1885A0`), DXVK's presenter creates an `800x600` `VK_FORMAT_R8G8B8A8_UNORM` `VK_PRESENT_MODE_FIFO_KHR` swapchain, compiles the BC7 sample's SPIR-V shaders on 2 compiler threads, submits command buffers via `vkQueueSubmit2`, and presents frames via `vkQueuePresentKHR`, cleanly posting `WM_DESTROY` after 3 presented frames.

---

## Master Patch Verification Table

| Target Binary | File Offset | Original Bytes | Patched Bytes | Purpose |
| :--- | :--- | :--- | :--- | :--- |
| `usr/lib/wine/aarch64-unix/ntdll.so` | `0x3c530` | `7f0006eb 23010054 0a000014 650440b9` | `c40240b9 9f1c0071 001e0054 09000014` | Map `SIGBUS` directly to `STATUS_DATATYPE_MISALIGNMENT` |
| `usr/lib/wine/aarch64-unix/ntdll.so` | `0x4dfc0` | `a00438d5` | `000080d2` | `mrs x0, ID_AA64SMFR0_EL1` -> `mov x0, #0` |
| `usr/lib/wine/aarch64-unix/ntdll.so` | `0x4e100` | `400738d5` | `000080d2` | `mrs x0, ID_AA64MMFR2_EL1` -> `mov x0, #0` |
| `usr/lib/wine/aarch64-unix/ntdll.so` | `0x5ec54` | `000200f0` | `1c000014` | Return `EEXIST` when `MAP_FIXED_NOREPLACE` fails |
| `usr/lib/wine/aarch64-unix/ntdll.so` | `0x61f48` | `2000a0d2` | `0000a4d2` | Start low reserve at `0x20000000` |
| `usr/lib/wine/aarch64-unix/ntdll.so` | `0x61f5c` | `e17b70b2` | `e17770b2` | High reserve end -> `0x3fffffff0000` |
| `usr/lib/wine/aarch64-unix/ntdll.so` | `0x61f60` | `e05767b2` | `e05367b2` | High reserve start -> `0x3ffffe000000` |
| `usr/lib/wine/aarch64-unix/ntdll.so` | `0x912f0`–`0x91300` | `0000ffffff7f0000` | `0000ffffff3f0000` | Clamp `.data` user space limits |
| `usr/lib/wine/aarch64-windows/wineboot.exe` | `0x1260c` | `00e4006f` | `d3080014` | Fast exit after CPU registry initialization |
| `usr/lib/wine/aarch64-windows/libarm64ecfex.dll` | `0x3068` | `f5700694` | `1f2003d5` | NOP premature `aligned_free` (UAF fix) |
| `usr/lib/wine/aarch64-unix/win32u.so` | `0x8a66c` | `413402f9` | `1f2003d5` | Preserve `null_driver.pCreateWindow` in `load_driver` |
| `usr/lib/wine/aarch64-unix/win32u.so` | `0x1885a0` | `620240b9 00008052 610a40b9 ...` (36 B) | `02648052 034b8052 7f7e0029 ...` (36 B) | Populate `800x600` `client_rect` in headless `get_client_rect` |
| `proot-qemu/qemu-aarch64-static` | `0x21f0e7` | `0f84d0020000` | `909090909090` | Handle JIT write faults with `REG_TRAPNO = 0` |
| `proot-qemu/qemu-aarch64-static` | `0x4660e9` | `49445f4141363449...` (`ID_AA64ISAR*`) | `49445f414136342a...` (`ID_AA64*`) | Universal CPUID user-mode read whitelist |
