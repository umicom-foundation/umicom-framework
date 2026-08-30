/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_profiler_filter.c
 *
 * PURPOSE:
 *   Implement the test profiler filter behavior for
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
#include "umicom/observability/performance/profiler_filter.h"

int main(void) {
    UmiPerformanceProfilerFilter left;
    UmiPerformanceProfilerFilter right;
    if (umi_performance_profiler_filter_init(&left, "profiler_filter", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_filter_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_filter_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_profiler_filter_transition_allowed(UMI_PERFORMANCE_STATE_IDLE, UMI_PERFORMANCE_STATE_ACTIVE)) return 4;
    if (umi_performance_profiler_filter_init(&right, "profiler_filter", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_filter_same_identity(&left, &right)) return 6;
    puts("profiler_filter: ok");
    return 0;
}
