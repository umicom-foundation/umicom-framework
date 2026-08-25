#include <stdio.h>
#include "umicom/observability/performance/profiler_process.h"

int main(void) {
    UmiPerformanceProfilerProcess left;
    UmiPerformanceProfilerProcess right;
    if (umi_performance_profiler_process_init(&left, "profiler_process", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_process_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_process_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_profiler_process_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_profiler_process_init(&right, "profiler_process", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_process_same_identity(&left, &right)) return 6;
    puts("profiler_process: ok");
    return 0;
}
