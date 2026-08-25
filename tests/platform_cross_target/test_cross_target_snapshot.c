/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_cross_target_snapshot.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the cross target snapshot cross-target capability.
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

#include "umicom/platform/cross_target/cross_target_snapshot.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtCrossTargetSnapshot s={0};s.target.architecture=UMI_CT_ARCH_RISCV64;CHECK(umi_ct_copy(s.abi,sizeof(s.abi),"lp64d")==UMI_STATUS_OK);s.cpu_count=4U;s.page_size=4096U;s.fingerprint=1U;s.health.health=UMI_CT_HEALTH_READY;CHECK(umi_ct_cross_target_snapshot_validate(&s)==UMI_STATUS_OK);s.health.health=UMI_CT_HEALTH_BLOCKED;CHECK(umi_ct_cross_target_snapshot_validate(&s)==UMI_STATUS_UNAVAILABLE);return 0;}
