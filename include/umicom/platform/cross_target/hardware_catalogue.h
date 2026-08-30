/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/hardware_catalogue.h
 *
 * PURPOSE:
 *   Maintain bounded hardware inventory and detect overlapping MMIO regions before driver activation.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_HARDWARE_CATALOGUE_H
#define UMICOM_PLATFORM_CROSS_TARGET_HARDWARE_CATALOGUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/hardware_device.h"
typedef struct UmiCtHardwareCatalogue { UmiCtHardwareDevice devices[UMI_CT_MAX_ITEMS]; size_t count; } UmiCtHardwareCatalogue;
void umi_ct_hardware_catalogue_init(UmiCtHardwareCatalogue *catalogue);
UmiStatus umi_ct_hardware_catalogue_add(UmiCtHardwareCatalogue *catalogue,const UmiCtHardwareDevice *device);
const UmiCtHardwareDevice *umi_ct_hardware_catalogue_find(const UmiCtHardwareCatalogue *catalogue,const char *device_id);
bool umi_ct_hardware_catalogue_mmio_conflict(const UmiCtHardwareCatalogue *catalogue,const UmiCtHardwareDevice *device);

#ifdef __cplusplus
}
#endif

#endif
