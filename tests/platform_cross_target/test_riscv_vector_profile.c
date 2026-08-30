/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_riscv_vector_profile.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the riscv vector profile cross-target capability.
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

#include "umicom/platform/cross_target/riscv_vector_profile.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtRiscvVectorProfile p={128U,64U,true};CHECK(umi_ct_riscv_vector_profile_satisfied(&p,256U,64U,true));CHECK(!umi_ct_riscv_vector_profile_satisfied(&p,64U,64U,true));return 0;}
