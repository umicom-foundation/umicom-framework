/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_performance_budget.c
 *
 * PURPOSE:
 *   Implement the test performance budget behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/observability/performance/performance_budget.h"

int main(void) {
    UmiPerformancePerformanceBudget left;
    UmiPerformancePerformanceBudget right;
    if (umi_performance_performance_budget_init(&left, "performance_budget", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_performance_budget_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_performance_budget_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_performance_budget_within_budget(5.0, 10.0)) return 4;
    if (umi_performance_performance_budget_init(&right, "performance_budget", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_performance_budget_same_identity(&left, &right)) return 6;
    puts("performance_budget: ok");
    return 0;
}
