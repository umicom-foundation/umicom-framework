/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/performance_budget.h
 *
 * PURPOSE:
 *   Represent performance regression and budget control state for performance budget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PERFORMANCE_BUDGET_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PERFORMANCE_BUDGET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformancePerformanceBudget {
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
} UmiPerformancePerformanceBudget;

/* Initialise a versioned performance budget record with stable identities. */
UmiStatus umi_performance_performance_budget_init(UmiPerformancePerformanceBudget *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_performance_budget_validate(const UmiPerformancePerformanceBudget *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_performance_budget_observe(UmiPerformancePerformanceBudget *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_performance_budget_same_identity(const UmiPerformancePerformanceBudget *left, const UmiPerformancePerformanceBudget *right);
/* Domain-specific policy helper for performance budget. */
bool umi_performance_performance_budget_within_budget(double used, double limit);

#ifdef __cplusplus
}
#endif
#endif
