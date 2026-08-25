#include <stdio.h>
#include "umicom/observability/performance/profiler_frame.h"

int main(void) {
    UmiPerformanceProfilerFrame left;
    UmiPerformanceProfilerFrame right;
    if (umi_performance_profiler_frame_init(&left, "profiler_frame", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_frame_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_frame_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_profiler_frame_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_profiler_frame_init(&right, "profiler_frame", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_frame_same_identity(&left, &right)) return 6;
    puts("profiler_frame: ok");
    return 0;
}
