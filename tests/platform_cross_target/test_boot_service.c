/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_boot_service.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the boot service cross-target capability.
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

#include "umicom/platform/cross_target/boot_service.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtBootService k={"kernel",UMI_CT_BOOT_EARLY,true,1000U},n={"net",UMI_CT_BOOT_SERVICES,true,2000U};CHECK(umi_ct_boot_service_validate(&k)==UMI_STATUS_OK);CHECK(umi_ct_boot_dependency_phase_valid(&n,&k));CHECK(!umi_ct_boot_dependency_phase_valid(&k,&n));return 0;}
