#include <stdio.h>
#include "umicom/observability/performance/profiler_sample_buffer.h"

int main(void) {
    UmiPerformanceProfilerSampleBuffer left;
    UmiPerformanceProfilerSampleBuffer right;
    if (umi_performance_profiler_sample_buffer_init(&left, "profiler_sample_buffer", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_sample_buffer_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_sample_buffer_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_profiler_sample_buffer_capacity_available(1U, 2U)) return 4;
    if (umi_performance_profiler_sample_buffer_init(&right, "profiler_sample_buffer", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_sample_buffer_same_identity(&left, &right)) return 6;
    puts("profiler_sample_buffer: ok");
    return 0;
}
