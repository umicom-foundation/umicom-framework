#include <stdio.h>
#include "umicom/observability/performance/profiler_filter.h"

int main(void) {
    UmiPerformanceProfilerFilter left;
    UmiPerformanceProfilerFilter right;
    if (umi_performance_profiler_filter_init(&left, "profiler_filter", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_filter_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_filter_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_profiler_filter_transition_allowed(UMI_PERFORMANCE_STATE_IDLE, UMI_PERFORMANCE_STATE_ACTIVE)) return 4;
    if (umi_performance_profiler_filter_init(&right, "profiler_filter", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_filter_same_identity(&left, &right)) return 6;
    puts("profiler_filter: ok");
    return 0;
}
