/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/timeline.h
 *
 * PURPOSE:
 *   Represent ordered operational timeline evidence for timeline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_TIMELINE_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_TIMELINE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance timeline data shared with callers of this public contract.
 */
typedef struct UmiPerformanceTimeline {
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
} UmiPerformanceTimeline;

/* Initialise a versioned timeline record with stable identities. */
UmiStatus umi_performance_timeline_init(UmiPerformanceTimeline *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_timeline_validate(const UmiPerformanceTimeline *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_timeline_observe(UmiPerformanceTimeline *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_timeline_same_identity(const UmiPerformanceTimeline *left, const UmiPerformanceTimeline *right);
/* Domain-specific policy helper for timeline. */
double umi_performance_timeline_weighted_score(double primary, double secondary, double secondary_weight);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_performance_timeline_archive_encode(const UmiPerformanceTimeline *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_performance_timeline_archive_decode(const void *bytes, size_t byte_count,
    UmiPerformanceTimeline *value);

#ifdef __cplusplus
}
#endif
#endif
