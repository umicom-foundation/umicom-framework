/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/gauge.h
 *
 * PURPOSE:
 *   Represent point-in-time gauge observations for operational telemetry for gauge.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_GAUGE_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_GAUGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceGauge {
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
} UmiPerformanceGauge;

/* Initialise a versioned gauge record with stable identities. */
UmiStatus umi_performance_gauge_init(UmiPerformanceGauge *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_gauge_validate(const UmiPerformanceGauge *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_gauge_observe(UmiPerformanceGauge *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_gauge_same_identity(const UmiPerformanceGauge *left, const UmiPerformanceGauge *right);
/* Domain-specific policy helper for gauge. */
bool umi_performance_gauge_exceeds(double value, double threshold);

#ifdef __cplusplus
}
#endif
#endif
