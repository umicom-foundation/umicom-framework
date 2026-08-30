/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/memory_budget.h
 *
 * PURPOSE:
 *   Represent memory pressure, budgets and regression policy for memory budget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_MEMORY_BUDGET_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_MEMORY_BUDGET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceMemoryBudget {
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
} UmiPerformanceMemoryBudget;

/* Initialise a versioned memory budget record with stable identities. */
UmiStatus umi_performance_memory_budget_init(UmiPerformanceMemoryBudget *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_memory_budget_validate(const UmiPerformanceMemoryBudget *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_memory_budget_observe(UmiPerformanceMemoryBudget *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_memory_budget_same_identity(const UmiPerformanceMemoryBudget *left, const UmiPerformanceMemoryBudget *right);
/* Domain-specific policy helper for memory budget. */
bool umi_performance_memory_budget_within_budget(double used, double limit);

#ifdef __cplusplus
}
#endif
#endif
