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
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named ct bus type values accepted by this public contract.
 */
typedef enum UmiCtBusType { UMI_CT_BUS_PLATFORM=1, UMI_CT_BUS_PCI=2, UMI_CT_BUS_VIRTIO=3, UMI_CT_BUS_USB=4, UMI_CT_BUS_I2C=5, UMI_CT_BUS_SPI=6 } UmiCtBusType;
/**
 * Represent the ct hardware bus data shared with callers of this public contract.
 */
typedef struct UmiCtHardwareBus { char bus_id[UMI_CT_ID_CAPACITY]; UmiCtBusType type; bool enumerable; bool hotplug; bool dma; uint32_t address_bits; } UmiCtHardwareBus;
/**
 * Check that ct hardware bus satisfies its contract before another service relies on it.
 */
UmiStatus umi_ct_hardware_bus_validate(const UmiCtHardwareBus *bus);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ct_hardware_bus_archive_encode(const UmiCtHardwareBus *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ct_hardware_bus_archive_decode(const void *bytes, size_t byte_count,
    UmiCtHardwareBus *value);

#ifdef __cplusplus
}
#endif

#endif
