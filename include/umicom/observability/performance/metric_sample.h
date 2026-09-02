/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/metric_sample.h
 *
 * PURPOSE:
 *   Represent typed metric metadata, samples and time-series state for metric sample.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_METRIC_SAMPLE_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_METRIC_SAMPLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance metric sample data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceMetricSample {
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
} UmiPerformanceMetricSample;

/* Initialise a versioned metric sample record with stable identities. */
UmiStatus umi_performance_metric_sample_init(UmiPerformanceMetricSample *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_metric_sample_validate(const UmiPerformanceMetricSample *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_metric_sample_observe(UmiPerformanceMetricSample *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_metric_sample_same_identity(const UmiPerformanceMetricSample *left, const UmiPerformanceMetricSample *right);
/* Domain-specific policy helper for metric sample. */
double umi_performance_metric_sample_mean(double sum, uint64_t count);

#ifdef __cplusplus
}
#endif
#endif
