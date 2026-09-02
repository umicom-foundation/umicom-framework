/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/interrupt_descriptor.h
 *
 * PURPOSE:
 *   Describe interrupt vectors, trigger mode and affinity hints independently of the concrete interrupt controller.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_INTERRUPT_DESCRIPTOR_H
#define UMICOM_PLATFORM_CROSS_TARGET_INTERRUPT_DESCRIPTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named ct interrupt trigger values accepted by this public contract.
 */
typedef enum UmiCtInterruptTrigger { UMI_CT_IRQ_EDGE=1, UMI_CT_IRQ_LEVEL=2 } UmiCtInterruptTrigger;
/**
 * Represent the ct interrupt descriptor data shared with callers of this public contract.
 */
typedef struct UmiCtInterruptDescriptor { uint32_t vector; uint8_t priority; UmiCtInterruptTrigger trigger; bool active_low; uint64_t affinity_mask; } UmiCtInterruptDescriptor;
/**
 * Check that ct interrupt descriptor satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ct_interrupt_descriptor_validate(const UmiCtInterruptDescriptor *descriptor,uint32_t maximum_vector);

#ifdef __cplusplus
}
#endif

#endif
