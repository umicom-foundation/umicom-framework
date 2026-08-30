/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_riscv_profile.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the riscv profile cross-target capability.
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

#include "umicom/platform/cross_target/riscv_profile.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtRiscvProfile p;UmiCtRiscvIsa a;CHECK(umi_ct_riscv_profile_builtin("umos-rv64",&p)==UMI_STATUS_OK);CHECK(umi_ct_riscv_isa_parse("rv64gc_zba_zbb_v",&a)==UMI_STATUS_OK);CHECK(umi_ct_riscv_profile_satisfied(&p,&a,4U,true));CHECK(!umi_ct_riscv_profile_satisfied(&p,&a,1U,true));return 0;}
