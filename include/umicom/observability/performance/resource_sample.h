/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/resource_sample.h
 *
 * PURPOSE:
 *   Represent process/system resource observations and budgets for resource sample.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_RESOURCE_SAMPLE_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_RESOURCE_SAMPLE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance resource sample data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceResourceSample {
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
} UmiPerformanceResourceSample;

/* Initialise a versioned resource sample record with stable identities. */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_performance_resource_sample_init(UmiPerformanceResourceSample *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_resource_sample_validate(const UmiPerformanceResourceSample *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_resource_sample_observe(UmiPerformanceResourceSample *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_resource_sample_same_identity(const UmiPerformanceResourceSample *left, const UmiPerformanceResourceSample *right);
/* Domain-specific policy helper for resource sample. */
double umi_performance_resource_sample_ratio(double numerator, double denominator);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_performance_resource_sample_archive_encode(const UmiPerformanceResourceSample *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_performance_resource_sample_archive_decode(const void *bytes, size_t byte_count,
    UmiPerformanceResourceSample *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiPerformanceResourceSample object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_performance_resource_sample_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiPerformanceResourceSample *workspace, UmiPerformanceResourceSample *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_performance_resource_sample_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPerformanceResourceSample *expected, UmiPerformanceResourceSample *value);


#ifdef __cplusplus
}
#endif
#endif
