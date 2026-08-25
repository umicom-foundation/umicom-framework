/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_target_profile.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the target profile cross-target capability.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/target_profile.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtTarget t={0};UmiCtTargetProfile p;t.architecture=UMI_CT_ARCH_RISCV64;CHECK(umi_ct_target_profile_init(&p,"umos.rv64",&t,"lp64d")==UMI_STATUS_OK);p.required_cpu_features=3U;CHECK(umi_ct_target_profile_satisfied(&p,7U,4096U));CHECK(!umi_ct_target_profile_satisfied(&p,1U,4096U));return 0;}
