#include <stdio.h>
#include "umicom/observability/performance/benchmark_run.h"

int main(void) {
    UmiPerformanceBenchmarkRun left;
    UmiPerformanceBenchmarkRun right;
    if (umi_performance_benchmark_run_init(&left, "benchmark_run", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_benchmark_run_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_benchmark_run_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_benchmark_run_duration_ns(10U, 25U) != 15U) return 4;
    if (umi_performance_benchmark_run_init(&right, "benchmark_run", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_benchmark_run_same_identity(&left, &right)) return 6;
    puts("benchmark_run: ok");
    return 0;
}
