/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/hardware_device.h
 *
 * PURPOSE:
 *   Describe hardware devices using bus-neutral MMIO/IRQ metadata for Umicom OS driver matching.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_HARDWARE_DEVICE_H
#define UMICOM_PLATFORM_CROSS_TARGET_HARDWARE_DEVICE_H

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
 * List the named ct device class values accepted by this public contract.
 */
typedef enum UmiCtDeviceClass { UMI_CT_DEVICE_CPU=1, UMI_CT_DEVICE_INTERRUPT=2, UMI_CT_DEVICE_TIMER=3, UMI_CT_DEVICE_STORAGE=4, UMI_CT_DEVICE_NETWORK=5, UMI_CT_DEVICE_DISPLAY=6, UMI_CT_DEVICE_INPUT=7, UMI_CT_DEVICE_OTHER=8 } UmiCtDeviceClass;
/**
 * Represent the ct hardware device data shared with callers of this public contract.
 */
typedef struct UmiCtHardwareDevice { char device_id[UMI_CT_ID_CAPACITY]; char compatible[UMI_CT_NAME_CAPACITY]; UmiCtDeviceClass device_class; uint64_t mmio_base; uint64_t mmio_size; uint32_t irq; bool present; } UmiCtHardwareDevice;
/**
 * Check that ct hardware device satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_hardware_device_validate(const UmiCtHardwareDevice *device);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ct_hardware_device_archive_encode(const UmiCtHardwareDevice *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ct_hardware_device_archive_decode(const void *bytes, size_t byte_count,
    UmiCtHardwareDevice *value);

#ifdef __cplusplus
}
#endif

#endif
