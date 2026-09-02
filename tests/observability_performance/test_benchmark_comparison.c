/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_benchmark_comparison.c
 *
 * PURPOSE:
 *   Implement the test benchmark comparison behavior for
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
#include "umicom/observability/performance/benchmark_comparison.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceBenchmarkComparison left;
    UmiPerformanceBenchmarkComparison right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_benchmark_comparison_init(&left, "benchmark_comparison", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_benchmark_comparison_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_benchmark_comparison_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_benchmark_comparison_regression_percent(120.0, 100.0) != 20.0) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_benchmark_comparison_init(&right, "benchmark_comparison", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_benchmark_comparison_same_identity(&left, &right)) return 6;
    puts("benchmark_comparison: ok");
    return 0;
}
