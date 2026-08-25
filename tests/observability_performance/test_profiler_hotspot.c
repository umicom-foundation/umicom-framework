#include <stdio.h>
#include "umicom/observability/performance/profiler_hotspot.h"

int main(void) {
    UmiPerformanceProfilerHotspot left;
    UmiPerformanceProfilerHotspot right;
    if (umi_performance_profiler_hotspot_init(&left, "profiler_hotspot", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_hotspot_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_hotspot_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_profiler_hotspot_exceeds(11.0, 10.0)) return 4;
    if (umi_performance_profiler_hotspot_init(&right, "profiler_hotspot", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_hotspot_same_identity(&left, &right)) return 6;
    puts("profiler_hotspot: ok");
    return 0;
}
