/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/runtime_health.h
 *
 * PURPOSE:
 *   Summarise runtime health from performance evidence for runtime health.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_RUNTIME_HEALTH_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_RUNTIME_HEALTH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceRuntimeHealth {
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
} UmiPerformanceRuntimeHealth;

/* Initialise a versioned runtime health record with stable identities. */
UmiStatus umi_performance_runtime_health_init(UmiPerformanceRuntimeHealth *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_runtime_health_validate(const UmiPerformanceRuntimeHealth *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_runtime_health_observe(UmiPerformanceRuntimeHealth *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_runtime_health_same_identity(const UmiPerformanceRuntimeHealth *left, const UmiPerformanceRuntimeHealth *right);
/* Domain-specific policy helper for runtime health. */
bool umi_performance_runtime_health_healthy(UmiPerformanceSeverity severity, bool failed);

#ifdef __cplusplus
}
#endif
#endif
