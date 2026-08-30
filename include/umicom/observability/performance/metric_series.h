/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/metric_series.h
 *
 * PURPOSE:
 *   Represent typed metric metadata, samples and time-series state for metric series.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_METRIC_SERIES_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_METRIC_SERIES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceMetricSeries {
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
} UmiPerformanceMetricSeries;

/* Initialise a versioned metric series record with stable identities. */
UmiStatus umi_performance_metric_series_init(UmiPerformanceMetricSeries *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_metric_series_validate(const UmiPerformanceMetricSeries *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_metric_series_observe(UmiPerformanceMetricSeries *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_metric_series_same_identity(const UmiPerformanceMetricSeries *left, const UmiPerformanceMetricSeries *right);
/* Domain-specific policy helper for metric series. */
bool umi_performance_metric_series_capacity_available(size_t count, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
