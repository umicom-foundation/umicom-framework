/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/memory_region.h
 *
 * PURPOSE:
 *   Describe physical/virtual memory regions and detect address-space overlap before boot allocation.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_MEMORY_REGION_H
#define UMICOM_PLATFORM_CROSS_TARGET_MEMORY_REGION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiCtRegionType { UMI_CT_REGION_RAM=1, UMI_CT_REGION_ROM=2, UMI_CT_REGION_MMIO=3, UMI_CT_REGION_RESERVED=4, UMI_CT_REGION_FIRMWARE=5 } UmiCtRegionType;
typedef uint32_t UmiCtRegionPermissions;
enum { UMI_CT_REGION_READ=1U<<0, UMI_CT_REGION_WRITE=1U<<1, UMI_CT_REGION_EXEC=1U<<2, UMI_CT_REGION_DEVICE=1U<<3 };
typedef struct UmiCtMemoryRegion { uint64_t base; uint64_t size; UmiCtRegionType type; UmiCtRegionPermissions permissions; } UmiCtMemoryRegion;
UmiStatus umi_ct_memory_region_validate(const UmiCtMemoryRegion *region);
bool umi_ct_memory_region_overlaps(const UmiCtMemoryRegion *left,const UmiCtMemoryRegion *right);

#ifdef __cplusplus
}
#endif

#endif
