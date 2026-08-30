/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_calling_convention.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the calling convention cross-target capability.
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

#include "umicom/platform/cross_target/calling_convention.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){CHECK(umi_ct_calling_convention_default(UMI_CT_ARCH_RISCV64,UMI_CT_OS_LINUX)==UMI_CT_CALL_RISCV);CHECK(umi_ct_calling_convention_default(UMI_CT_ARCH_X86_64,UMI_CT_OS_WINDOWS)==UMI_CT_CALL_WIN64);CHECK(umi_ct_calling_convention_default(UMI_CT_ARCH_RISCV64,UMI_CT_OS_UMICOM)==UMI_CT_CALL_UMICOM);return 0;}
