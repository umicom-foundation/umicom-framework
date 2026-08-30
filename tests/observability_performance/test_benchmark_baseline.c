/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_benchmark_baseline.c
 *
 * PURPOSE:
 *   Implement the test benchmark baseline behavior for
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
#include "umicom/observability/performance/benchmark_baseline.h"

int main(void) {
    UmiPerformanceBenchmarkBaseline left;
    UmiPerformanceBenchmarkBaseline right;
    if (umi_performance_benchmark_baseline_init(&left, "benchmark_baseline", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_benchmark_baseline_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_benchmark_baseline_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_benchmark_baseline_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_benchmark_baseline_init(&right, "benchmark_baseline", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_benchmark_baseline_same_identity(&left, &right)) return 6;
    puts("benchmark_baseline: ok");
    return 0;
}
