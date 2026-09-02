/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_riscv_memory_model.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the riscv memory model cross-target capability.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/riscv_memory_model.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtRiscvMemoryModel m={UMI_CT_RISCV_MMU_SV39,56U,39U,4096U,true};CHECK(umi_ct_riscv_memory_model_validate(&m,64U)==UMI_STATUS_OK);CHECK(umi_ct_riscv_memory_model_validate(&m,32U)==UMI_STATUS_INVALID_STATE);return 0;}
