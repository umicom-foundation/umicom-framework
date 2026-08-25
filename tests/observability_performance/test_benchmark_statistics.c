#include <stdio.h>
#include "umicom/observability/performance/benchmark_statistics.h"

int main(void) {
    UmiPerformanceBenchmarkStatistics left;
    UmiPerformanceBenchmarkStatistics right;
    if (umi_performance_benchmark_statistics_init(&left, "benchmark_statistics", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_benchmark_statistics_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_benchmark_statistics_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_benchmark_statistics_mean(10.0, 2U) != 5.0) return 4;
    if (umi_performance_benchmark_statistics_init(&right, "benchmark_statistics", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_benchmark_statistics_same_identity(&left, &right)) return 6;
    puts("benchmark_statistics: ok");
    return 0;
}
