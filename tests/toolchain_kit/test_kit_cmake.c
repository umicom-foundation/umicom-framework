/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_kit/test_kit_cmake.c
 *
 * PURPOSE:
 *   Implement the test kit cmake behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | CMake toolchain rendering test | Sammy Hegab | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/kit_cmake.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiToolchainKitSnapshot kit;
    char text[16384];
    umi_toolchain_kit_init(&kit, "kit.riscv", "RISC-V");
    (void)strcpy(kit.profile_id, "riscv");
    (void)strcpy(kit.host_triple, "x86_64-w64-mingw32");
    (void)strcpy(kit.target_triple, "riscv64-unknown-elf");
    (void)strcpy(kit.c_compiler, "C:/tool/riscv64-unknown-elf-gcc.exe");
    (void)strcpy(kit.assembler, "C:/tool/riscv64-unknown-elf-as.exe");
    (void)strcpy(kit.linker, "C:/tool/riscv64-unknown-elf-ld.exe");
    (void)strcpy(kit.sysroot, "C:/tool/riscv64-unknown-elf");
    assert(umi_toolchain_kit_cmake_render(&kit, text, sizeof(text)) == UMI_STATUS_OK);
    assert(strstr(text, "CMAKE_SYSTEM_PROCESSOR \"riscv64\"") != NULL);
    assert(strstr(text, "CMAKE_C_COMPILER_TARGET \"riscv64-unknown-elf\"") != NULL);
    return 0;
}
