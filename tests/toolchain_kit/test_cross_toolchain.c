/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_kit/test_cross_toolchain.c
 *
 * PURPOSE:
 *   Implement the test cross toolchain behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | cross toolchain planning test | Sammy Hegab | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/cross_toolchain.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCrossToolchainRequest request;
    UmiCrossToolchainPlan plan;
    umi_cross_toolchain_request_init(&request);
    request.family = UMI_CROSS_TOOLCHAIN_GNU;
    request.target_triple = "riscv64-unknown-elf";
    assert(umi_cross_toolchain_plan(&request, &plan) == UMI_STATUS_OK);
    assert(strcmp(plan.compiler, "riscv64-unknown-elf-gcc") == 0);
    assert(strcmp(plan.assembler, "riscv64-unknown-elf-as") == 0);
    assert(strcmp(plan.linker, "riscv64-unknown-elf-ld") == 0);
    assert(strcmp(plan.debugger, "riscv64-unknown-elf-gdb") == 0);

    request.family = UMI_CROSS_TOOLCHAIN_CLANG;
    assert(umi_cross_toolchain_plan(&request, &plan) == UMI_STATUS_OK);
    assert(strcmp(plan.compiler, "clang") == 0);
    assert(strcmp(plan.target_argument, "--target=riscv64-unknown-elf") == 0);
    return 0;
}
