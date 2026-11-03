#include <windows.h>
#include <stdio.h>
int main(void) {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    printf("Hello from Windows x86-64 on wine-aarch64 via ARM64EC/FEX! Arch=%u\n", (unsigned int)si.wProcessorArchitecture);
    fflush(stdout);
    return 0;
}
