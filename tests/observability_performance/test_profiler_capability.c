/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_profiler_capability.c
 *
 * PURPOSE:
 *   Implement the test profiler capability behavior for
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
#include "umicom/observability/performance/profiler_capability.h"

int main(void) {
    UmiPerformanceProfilerCapability left;
    UmiPerformanceProfilerCapability right;
    if (umi_performance_profiler_capability_init(&left, "profiler_capability", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_profiler_capability_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_profiler_capability_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_profiler_capability_transition_allowed(UMI_PERFORMANCE_STATE_IDLE, UMI_PERFORMANCE_STATE_ACTIVE)) return 4;
    if (umi_performance_profiler_capability_init(&right, "profiler_capability", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_profiler_capability_same_identity(&left, &right)) return 6;
    puts("profiler_capability: ok");
    return 0;
}
