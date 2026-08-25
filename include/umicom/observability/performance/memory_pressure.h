/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/memory_pressure.h
 *
 * PURPOSE:
 *   Represent memory pressure, budgets and regression policy for memory pressure.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_MEMORY_PRESSURE_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_MEMORY_PRESSURE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceMemoryPressure {
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
} UmiPerformanceMemoryPressure;

/* Initialise a versioned memory pressure record with stable identities. */
UmiStatus umi_performance_memory_pressure_init(UmiPerformanceMemoryPressure *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_memory_pressure_validate(const UmiPerformanceMemoryPressure *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_memory_pressure_observe(UmiPerformanceMemoryPressure *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_memory_pressure_same_identity(const UmiPerformanceMemoryPressure *left, const UmiPerformanceMemoryPressure *right);
/* Domain-specific policy helper for memory pressure. */
bool umi_performance_memory_pressure_exceeds(double value, double threshold);

#ifdef __cplusplus
}
#endif
#endif
