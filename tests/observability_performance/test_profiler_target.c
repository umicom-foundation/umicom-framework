/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_profiler_target.c
 *
 * PURPOSE:
 *   Implement the test profiler target behavior for
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
#include "umicom/observability/performance/profiler_target.h"

int main(void) {
    UmiPerformanceProfilerTarget left;
    UmiPerformanceProfilerTarget right;
    if (umi_performance_profiler_target_init(&left, "profiler_target", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_target_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_target_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_profiler_target_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_profiler_target_init(&right, "profiler_target", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_target_same_identity(&left, &right)) return 6;
    puts("profiler_target: ok");
    return 0;
}
