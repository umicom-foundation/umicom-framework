/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_benchmark_sample.c
 *
 * PURPOSE:
 *   Implement the test benchmark sample behavior for
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
#include "umicom/observability/performance/benchmark_sample.h"

int main(void) {
    UmiPerformanceBenchmarkSample left;
    UmiPerformanceBenchmarkSample right;
    if (umi_performance_benchmark_sample_init(&left, "benchmark_sample", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_benchmark_sample_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_benchmark_sample_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_benchmark_sample_mean(10.0, 2U) != 5.0) return 4;
    if (umi_performance_benchmark_sample_init(&right, "benchmark_sample", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_benchmark_sample_same_identity(&left, &right)) return 6;
    puts("benchmark_sample: ok");
    return 0;
}
