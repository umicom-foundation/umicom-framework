/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/crash_correlation.h
 *
 * PURPOSE:
 *   Represent crash signatures, correlations and clustering evidence for crash correlation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_CRASH_CORRELATION_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_CRASH_CORRELATION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceCrashCorrelation {
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
} UmiPerformanceCrashCorrelation;

/* Initialise a versioned crash correlation record with stable identities. */
UmiStatus umi_performance_crash_correlation_init(UmiPerformanceCrashCorrelation *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_crash_correlation_validate(const UmiPerformanceCrashCorrelation *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_crash_correlation_observe(UmiPerformanceCrashCorrelation *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_crash_correlation_same_identity(const UmiPerformanceCrashCorrelation *left, const UmiPerformanceCrashCorrelation *right);
/* Domain-specific policy helper for crash correlation. */
double umi_performance_crash_correlation_ratio(double numerator, double denominator);

#ifdef __cplusplus
}
#endif
#endif
