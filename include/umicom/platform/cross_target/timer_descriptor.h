/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/timer_descriptor.h
 *
 * PURPOSE:
 *   Describe platform timer frequency, width and monotonic/oneshot properties for scheduler portability.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_TIMER_DESCRIPTOR_H
#define UMICOM_PLATFORM_CROSS_TARGET_TIMER_DESCRIPTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ct timer descriptor data shared with callers of this public contract.
 */
typedef struct UmiCtTimerDescriptor { char timer_id[UMI_CT_ID_CAPACITY]; uint64_t frequency_hz; uint32_t counter_bits; bool monotonic; bool oneshot; bool per_cpu; } UmiCtTimerDescriptor;
/**
 * Check that ct timer descriptor satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_timer_descriptor_validate(const UmiCtTimerDescriptor *descriptor);
/**
 * Provide the ct timer ns to ticks operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_timer_ns_to_ticks(const UmiCtTimerDescriptor *descriptor,uint64_t nanoseconds);

#ifdef __cplusplus
}
#endif

#endif
