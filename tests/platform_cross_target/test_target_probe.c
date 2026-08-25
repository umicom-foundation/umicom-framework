/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_target_probe.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the target probe cross-target capability.
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

#include "umicom/platform/cross_target/target_probe.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtTargetProbe p={0};UmiCtTarget e={0};p.target.architecture=e.architecture=UMI_CT_ARCH_RISCV64;p.target.operating_system=e.operating_system=UMI_CT_OS_UMICOM;p.target.environment=e.environment=UMI_CT_ENV_UMICOM;p.target.pointer_bits=e.pointer_bits=64U;p.cpu_count=4U;p.page_size=4096U;p.confidence=100U;CHECK(umi_ct_target_probe_validate(&p)==UMI_STATUS_OK);CHECK(umi_ct_target_probe_score(&p,&e)==100U);return 0;}
