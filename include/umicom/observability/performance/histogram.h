/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/histogram.h
 *
 * PURPOSE:
 *   Represent distribution summary observations for operational telemetry for histogram.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_HISTOGRAM_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_HISTOGRAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceHistogram {
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
} UmiPerformanceHistogram;

/* Initialise a versioned histogram record with stable identities. */
UmiStatus umi_performance_histogram_init(UmiPerformanceHistogram *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_histogram_validate(const UmiPerformanceHistogram *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_histogram_observe(UmiPerformanceHistogram *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_histogram_same_identity(const UmiPerformanceHistogram *left, const UmiPerformanceHistogram *right);
/* Domain-specific policy helper for histogram. */
bool umi_performance_histogram_exceeds(double value, double threshold);

#ifdef __cplusplus
}
#endif
#endif
