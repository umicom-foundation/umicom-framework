/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/profiler_hotspot.h
 *
 * PURPOSE:
 *   Represent and evaluate reusable CPU/process profiling state for profiler hotspot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_HOTSPOT_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_HOTSPOT_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance profiler hotspot data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceProfilerHotspot {
    uint32_t structure_size;
    uint32_t api_version;
    char id[UMI_PERFORMANCE_ID_CAPACITY];
    char subject_id[UMI_PERFORMANCE_ID_CAPACITY];
    UmiPerformanceState state;
    UmiPerformanceSeverity severity;
    uint64_t sequence;
    uint64_t timestamp_ns;
    double value;
    double auxiliary;
    uint64_t count;
    bool enabled;
} UmiPerformanceProfilerHotspot;

/* Initialise a versioned profiler hotspot record with stable identities. */
UmiStatus umi_performance_profiler_hotspot_init(UmiPerformanceProfilerHotspot *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_profiler_hotspot_validate(const UmiPerformanceProfilerHotspot *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_profiler_hotspot_observe(UmiPerformanceProfilerHotspot *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_profiler_hotspot_same_identity(const UmiPerformanceProfilerHotspot *left, const UmiPerformanceProfilerHotspot *right);
/* Domain-specific policy helper for profiler hotspot. */
bool umi_performance_profiler_hotspot_exceeds(double value, double threshold);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_performance_profiler_hotspot_archive_encode(const UmiPerformanceProfilerHotspot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_performance_profiler_hotspot_archive_decode(const void *bytes, size_t byte_count,
    UmiPerformanceProfilerHotspot *value);

#ifdef __cplusplus
}
#endif
#endif
