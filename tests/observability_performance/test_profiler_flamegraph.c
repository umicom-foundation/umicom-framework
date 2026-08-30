/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_profiler_flamegraph.c
 *
 * PURPOSE:
 *   Implement the test profiler flamegraph behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/observability/performance/profiler_flamegraph.h"

int main(void) {
    UmiPerformanceProfilerFlamegraph left;
    UmiPerformanceProfilerFlamegraph right;
    if (umi_performance_profiler_flamegraph_init(&left, "profiler_flamegraph", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_flamegraph_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_flamegraph_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_profiler_flamegraph_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_profiler_flamegraph_init(&right, "profiler_flamegraph", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_flamegraph_same_identity(&left, &right)) return 6;
    puts("profiler_flamegraph: ok");
    return 0;
}
