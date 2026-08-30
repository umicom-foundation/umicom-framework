/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/hardware_device.c
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

#include "umicom/platform/cross_target/hardware_device.h"

UmiStatus umi_ct_hardware_device_validate(const UmiCtHardwareDevice*d){if(d==NULL||!umi_ct_id_valid(d->device_id)||d->device_class<UMI_CT_DEVICE_CPU||d->device_class>UMI_CT_DEVICE_OTHER)return UMI_STATUS_INVALID_ARGUMENT;if(d->mmio_size!=0U&&d->mmio_base+d->mmio_size<d->mmio_base)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
