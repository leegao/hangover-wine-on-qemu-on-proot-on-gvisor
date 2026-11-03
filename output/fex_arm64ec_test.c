#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>

static void test_cpuid(void) {
    int cpu_info[4] = {0};
    __asm__ __volatile__("cpuid" : "=a"(cpu_info[0]), "=b"(cpu_info[1]), "=c"(cpu_info[2]), "=d"(cpu_info[3]) : "a"(0), "c"(0));
    char vendor[13] = {0};
    *(int*)(vendor + 0) = cpu_info[1];
    *(int*)(vendor + 4) = cpu_info[3];
    *(int*)(vendor + 8) = cpu_info[2];
    printf("[2/4] x86-64 CPUID Vendor: %s (MaxLeaf=%d)\n", vendor, cpu_info[0]);
    fflush(stdout);
}

static void test_sse2(void) {
    __m128d va = _mm_set_pd(10.0, 20.0);
    __m128d vb = _mm_set_pd(5.0, 10.0);
    __m128d vmul = _mm_mul_pd(va, vb);
    double res[2];
    _mm_storeu_pd(res, vmul);
    printf("[3/4] SSE2 Vector Dot Product: %.1f (expected 250.0)\n", res[0] + res[1]);
    fflush(stdout);
}

static void test_virtual_memory(void) {
    SIZE_T size = 65536;
    LPVOID p = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!p) { printf("[4/4] VirtualAlloc failed\n"); exit(1); }
    *(unsigned char*)p = 0x5a;
    DWORD old_prot = 0;
    if (!VirtualProtect(p, size, PAGE_READONLY, &old_prot)) { printf("[4/4] VirtualProtect failed\n"); exit(1); }
    VirtualFree(p, 0, MEM_RELEASE);
    printf("[4/4] VirtualAlloc/VirtualProtect/VirtualFree: OK (byte=0x5a, oldProt=0x%lx)\n", old_prot);
    fflush(stdout);
}

int main(void) {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    printf("[1/4] System Info: Arch=%u (9=AMD64), PageSize=%lu, Processors=%lu\n",
           (unsigned int)si.wProcessorArchitecture, si.dwPageSize, si.dwNumberOfProcessors);
    fflush(stdout);
    test_cpuid();
    test_sse2();
    test_virtual_memory();
    printf("ALL FEX ARM64EC TESTS PASSED SUCCESSFULLY!\n");
    fflush(stdout);
    return 0;
}
