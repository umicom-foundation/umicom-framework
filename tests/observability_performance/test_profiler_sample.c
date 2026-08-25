#include <stdio.h>
#include "umicom/observability/performance/profiler_sample.h"

int main(void) {
    UmiPerformanceProfilerSample left;
    UmiPerformanceProfilerSample right;
    if (umi_performance_profiler_sample_init(&left, "profiler_sample", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_sample_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_sample_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_profiler_sample_exceeds(11.0, 10.0)) return 4;
    if (umi_performance_profiler_sample_init(&right, "profiler_sample", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_sample_same_identity(&left, &right)) return 6;
    puts("profiler_sample: ok");
    return 0;
}
