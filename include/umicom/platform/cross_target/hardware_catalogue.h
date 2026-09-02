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
/**
 * Represent the ct hardware catalogue data shared with callers of this public contract.
 */
typedef struct UmiCtHardwareCatalogue { UmiCtHardwareDevice devices[UMI_CT_MAX_ITEMS]; size_t count; } UmiCtHardwareCatalogue;
/**
 * Initialise ct hardware catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_ct_hardware_catalogue_init(UmiCtHardwareCatalogue *catalogue);
/**
 * Add ct hardware catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ct_hardware_catalogue_add(UmiCtHardwareCatalogue *catalogue,const UmiCtHardwareDevice *device);
/**
 * Find ct hardware catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCtHardwareDevice *umi_ct_hardware_catalogue_find(const UmiCtHardwareCatalogue *catalogue,const char *device_id);
/**
 * Provide the ct hardware catalogue mmio conflict operation used by this module and its
 * client applications.
 */
bool umi_ct_hardware_catalogue_mmio_conflict(const UmiCtHardwareCatalogue *catalogue,const UmiCtHardwareDevice *device);

#ifdef __cplusplus
}
#endif

#endif
