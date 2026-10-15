/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/scheduler_profile.h
 *
 * PURPOSE:
 *   Describe portable scheduler policy inputs for Umicom OS and runtime conformance.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_SCHEDULER_PROFILE_H
#define UMICOM_PLATFORM_CROSS_TARGET_SCHEDULER_PROFILE_H

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
 * List the named ct scheduler class values accepted by this public contract.
 */
typedef enum UmiCtSchedulerClass { UMI_CT_SCHED_COOPERATIVE=1, UMI_CT_SCHED_PREEMPTIVE=2, UMI_CT_SCHED_REALTIME=3 } UmiCtSchedulerClass;
/**
 * Represent the ct scheduler profile data shared with callers of this public contract.
 */
typedef struct UmiCtSchedulerProfile { UmiCtSchedulerClass scheduler_class; uint32_t cpu_count; uint32_t timeslice_us; uint8_t priority_levels; bool affinity; bool load_balancing; } UmiCtSchedulerProfile;
/**
 * Check that ct scheduler profile satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_scheduler_profile_validate(const UmiCtSchedulerProfile *profile);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ct_scheduler_profile_archive_encode(const UmiCtSchedulerProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ct_scheduler_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiCtSchedulerProfile *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ct_scheduler_profile_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiCtSchedulerProfile *expected, UmiCtSchedulerProfile *value);


#ifdef __cplusplus
}
#endif

#endif
