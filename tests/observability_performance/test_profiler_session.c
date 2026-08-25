#include <stdio.h>
#include "umicom/observability/performance/profiler_session.h"

int main(void) {
    UmiPerformanceProfilerSession left;
    UmiPerformanceProfilerSession right;
    if (umi_performance_profiler_session_init(&left, "profiler_session", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_session_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_session_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_profiler_session_duration_ns(10U, 25U) != 15U) return 4;
    if (umi_performance_profiler_session_init(&right, "profiler_session", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_session_same_identity(&left, &right)) return 6;
    puts("profiler_session: ok");
    return 0;
}
