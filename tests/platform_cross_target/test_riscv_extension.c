/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_riscv_extension.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the riscv extension cross-target capability.
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

#include "umicom/platform/cross_target/riscv_extension.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtCpuFeature f;CHECK(umi_ct_riscv_extension_feature("v",&f)==UMI_STATUS_OK);CHECK(f==UMI_CT_CPU_VECTOR);CHECK(umi_ct_riscv_extension_known("zicsr"));CHECK(!umi_ct_riscv_extension_known("xyz"));return 0;}
