/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_kit/test_kit_environment.c
 *
 * PURPOSE:
 *   Implement the test kit environment behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | kit environment test | Sammy Hegab | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/kit_environment.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiToolchainKitSnapshot kit;
    UmiEnvironmentPlan plan;
    umi_toolchain_kit_init(&kit, "kit.riscv", "RISC-V");
    (void)strcpy(kit.profile_id, "riscv");
    (void)strcpy(kit.host_triple, "x86_64-w64-mingw32");
    (void)strcpy(kit.target_triple, "riscv64-unknown-elf");
    (void)strcpy(kit.c_compiler, "C:/tool/riscv64-unknown-elf-gcc.exe");
    (void)strcpy(kit.linker, "C:/tool/riscv64-unknown-elf-ld.exe");
    (void)strcpy(kit.sysroot, "C:/tool/riscv64-unknown-elf");
    assert(umi_toolchain_kit_environment_plan(&kit, NULL, &plan) == UMI_STATUS_OK);
    assert(strcmp(umi_environment_plan_find(&plan, "CC"),
                  "C:/tool/riscv64-unknown-elf-gcc.exe") == 0);
    assert(strcmp(umi_environment_plan_find(&plan, "UMICOM_TARGET_TRIPLE"),
                  "riscv64-unknown-elf") == 0);
    return 0;
}
