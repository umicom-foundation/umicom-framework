/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/metric_descriptor.h
 *
 * PURPOSE:
 *   Represent typed metric metadata, samples and time-series state for metric descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_METRIC_DESCRIPTOR_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_METRIC_DESCRIPTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceMetricDescriptor {
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
} UmiPerformanceMetricDescriptor;

/* Initialise a versioned metric descriptor record with stable identities. */
UmiStatus umi_performance_metric_descriptor_init(UmiPerformanceMetricDescriptor *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_metric_descriptor_validate(const UmiPerformanceMetricDescriptor *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_metric_descriptor_observe(UmiPerformanceMetricDescriptor *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_metric_descriptor_same_identity(const UmiPerformanceMetricDescriptor *left, const UmiPerformanceMetricDescriptor *right);
/* Domain-specific policy helper for metric descriptor. */
double umi_performance_metric_descriptor_weighted_score(double primary, double secondary, double secondary_weight);

#ifdef __cplusplus
}
#endif
#endif
