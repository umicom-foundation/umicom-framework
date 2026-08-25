/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_memory_region.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the memory region cross-target capability.
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

#include "umicom/platform/cross_target/memory_region.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtMemoryRegion a={0x1000U,0x1000U,UMI_CT_REGION_RAM,UMI_CT_REGION_READ|UMI_CT_REGION_WRITE},b={0x1800U,0x100U,UMI_CT_REGION_MMIO,UMI_CT_REGION_READ};CHECK(umi_ct_memory_region_validate(&a)==UMI_STATUS_OK);CHECK(umi_ct_memory_region_overlaps(&a,&b));return 0;}
