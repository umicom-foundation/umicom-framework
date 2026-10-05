/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/clock_semantics.h
 *
 * PURPOSE:
 *   Describe monotonic/wall-clock availability and timer resolution for scheduler and profiling portability.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_CLOCK_SEMANTICS_H
#define UMICOM_PLATFORM_CROSS_TARGET_CLOCK_SEMANTICS_H

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
 * Represent the ct clock semantics data shared with callers of this public contract.
 */
typedef struct UmiCtClockSemantics { bool monotonic; bool wall_clock; bool high_resolution; uint64_t frequency_hz; uint64_t resolution_ns; } UmiCtClockSemantics;
/**
 * Check that ct clock semantics satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_clock_semantics_validate(const UmiCtClockSemantics *semantics);
/**
 * Provide the ct clock ticks to ns operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_clock_ticks_to_ns(const UmiCtClockSemantics *semantics,uint64_t ticks);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ct_clock_semantics_archive_encode(const UmiCtClockSemantics *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ct_clock_semantics_archive_decode(const void *bytes, size_t byte_count,
    UmiCtClockSemantics *value);

#ifdef __cplusplus
}
#endif

#endif
