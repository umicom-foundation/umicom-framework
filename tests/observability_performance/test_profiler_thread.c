/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_profiler_thread.c
 *
 * PURPOSE:
 *   Implement the test profiler thread behavior for
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
#include "umicom/observability/performance/profiler_thread.h"

int main(void) {
    UmiPerformanceProfilerThread left;
    UmiPerformanceProfilerThread right;
    if (umi_performance_profiler_thread_init(&left, "profiler_thread", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_thread_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_thread_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_profiler_thread_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_profiler_thread_init(&right, "profiler_thread", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_thread_same_identity(&left, &right)) return 6;
    puts("profiler_thread: ok");
    return 0;
}
