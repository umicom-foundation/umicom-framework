/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/memory_regression.h
 *
 * PURPOSE:
 *   Represent memory pressure, budgets and regression policy for memory regression.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_MEMORY_REGRESSION_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_MEMORY_REGRESSION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance memory regression data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceMemoryRegression {
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
} UmiPerformanceMemoryRegression;

/* Initialise a versioned memory regression record with stable identities. */
UmiStatus umi_performance_memory_regression_init(UmiPerformanceMemoryRegression *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_memory_regression_validate(const UmiPerformanceMemoryRegression *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_memory_regression_observe(UmiPerformanceMemoryRegression *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_memory_regression_same_identity(const UmiPerformanceMemoryRegression *left, const UmiPerformanceMemoryRegression *right);
/* Domain-specific policy helper for memory regression. */
double umi_performance_memory_regression_regression_percent(double current, double baseline);

#ifdef __cplusplus
}
#endif
#endif
