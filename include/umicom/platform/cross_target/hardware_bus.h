/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/hardware_bus.h
 *
 * PURPOSE:
 *   Describe discoverable hardware buses and address/interrupt translation capabilities.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_HARDWARE_BUS_H
#define UMICOM_PLATFORM_CROSS_TARGET_HARDWARE_BUS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiCtBusType { UMI_CT_BUS_PLATFORM=1, UMI_CT_BUS_PCI=2, UMI_CT_BUS_VIRTIO=3, UMI_CT_BUS_USB=4, UMI_CT_BUS_I2C=5, UMI_CT_BUS_SPI=6 } UmiCtBusType;
typedef struct UmiCtHardwareBus { char bus_id[UMI_CT_ID_CAPACITY]; UmiCtBusType type; bool enumerable; bool hotplug; bool dma; uint32_t address_bits; } UmiCtHardwareBus;
UmiStatus umi_ct_hardware_bus_validate(const UmiCtHardwareBus *bus);

#ifdef __cplusplus
}
#endif

#endif
