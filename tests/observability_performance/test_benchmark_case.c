#include <stdio.h>
#include "umicom/observability/performance/benchmark_case.h"

int main(void) {
    UmiPerformanceBenchmarkCase left;
    UmiPerformanceBenchmarkCase right;
    if (umi_performance_benchmark_case_init(&left, "benchmark_case", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_benchmark_case_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_benchmark_case_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_benchmark_case_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_benchmark_case_init(&right, "benchmark_case", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_benchmark_case_same_identity(&left, &right)) return 6;
    puts("benchmark_case: ok");
    return 0;
}
