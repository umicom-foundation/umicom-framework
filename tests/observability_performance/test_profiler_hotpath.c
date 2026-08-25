#include <stdio.h>
#include "umicom/observability/performance/profiler_hotpath.h"

int main(void) {
    UmiPerformanceProfilerHotpath left;
    UmiPerformanceProfilerHotpath right;
    if (umi_performance_profiler_hotpath_init(&left, "profiler_hotpath", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_hotpath_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_hotpath_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_profiler_hotpath_exceeds(11.0, 10.0)) return 4;
    if (umi_performance_profiler_hotpath_init(&right, "profiler_hotpath", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_hotpath_same_identity(&left, &right)) return 6;
    puts("profiler_hotpath: ok");
    return 0;
}
