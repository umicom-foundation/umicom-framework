/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_riscv_abi.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the riscv abi cross-target capability.
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

#include "umicom/platform/cross_target/riscv_abi.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtAbiDescriptor d;CHECK(umi_ct_riscv_abi_descriptor("lp64d",&d)==UMI_STATUS_OK);CHECK(d.hard_float&&d.pointer_bits==64U);CHECK(umi_ct_riscv_abi_matches_xlen(&d,64U));CHECK(!umi_ct_riscv_abi_matches_xlen(&d,32U));return 0;}
