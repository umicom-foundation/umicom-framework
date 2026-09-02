/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_memory_regression.c
 *
 * PURPOSE:
 *   Implement the test memory regression behavior for
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
#include "umicom/observability/performance/memory_regression.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceMemoryRegression left;
    UmiPerformanceMemoryRegression right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_memory_regression_init(&left, "memory_regression", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_memory_regression_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_memory_regression_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_memory_regression_regression_percent(120.0, 100.0) != 20.0) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_memory_regression_init(&right, "memory_regression", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_memory_regression_same_identity(&left, &right)) return 6;
    puts("memory_regression: ok");
    return 0;
}
