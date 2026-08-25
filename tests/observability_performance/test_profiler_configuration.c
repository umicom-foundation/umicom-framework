#include <stdio.h>
#include "umicom/observability/performance/profiler_configuration.h"

int main(void) {
    UmiPerformanceProfilerConfiguration left;
    UmiPerformanceProfilerConfiguration right;
    if (umi_performance_profiler_configuration_init(&left, "profiler_configuration", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_configuration_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_configuration_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_profiler_configuration_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_profiler_configuration_init(&right, "profiler_configuration", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_configuration_same_identity(&left, &right)) return 6;
    puts("profiler_configuration: ok");
    return 0;
}
