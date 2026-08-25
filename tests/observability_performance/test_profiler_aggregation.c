#include <stdio.h>
#include "umicom/observability/performance/profiler_aggregation.h"

int main(void) {
    UmiPerformanceProfilerAggregation left;
    UmiPerformanceProfilerAggregation right;
    if (umi_performance_profiler_aggregation_init(&left, "profiler_aggregation", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_aggregation_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_aggregation_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_profiler_aggregation_mean(10.0, 2U) != 5.0) return 4;
    if (umi_performance_profiler_aggregation_init(&right, "profiler_aggregation", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_aggregation_same_identity(&left, &right)) return 6;
    puts("profiler_aggregation: ok");
    return 0;
}
