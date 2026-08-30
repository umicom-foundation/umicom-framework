/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/resource_budget.h
 *
 * PURPOSE:
 *   Represent process/system resource observations and budgets for resource budget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_RESOURCE_BUDGET_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_RESOURCE_BUDGET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceResourceBudget {
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
} UmiPerformanceResourceBudget;

/* Initialise a versioned resource budget record with stable identities. */
UmiStatus umi_performance_resource_budget_init(UmiPerformanceResourceBudget *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_resource_budget_validate(const UmiPerformanceResourceBudget *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_resource_budget_observe(UmiPerformanceResourceBudget *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_resource_budget_same_identity(const UmiPerformanceResourceBudget *left, const UmiPerformanceResourceBudget *right);
/* Domain-specific policy helper for resource budget. */
bool umi_performance_resource_budget_within_budget(double used, double limit);

#ifdef __cplusplus
}
#endif
#endif
