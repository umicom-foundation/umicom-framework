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
#include "umicom/base/value_archive.h"
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ct_timer_descriptor_archive_encode(const UmiCtTimerDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ct_timer_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiCtTimerDescriptor *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiCtTimerDescriptor object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_ct_timer_descriptor_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiCtTimerDescriptor *workspace, UmiCtTimerDescriptor *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ct_timer_descriptor_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiCtTimerDescriptor *expected, UmiCtTimerDescriptor *value);


#ifdef __cplusplus
}
#endif

#endif
