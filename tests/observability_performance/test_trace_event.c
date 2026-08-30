/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_trace_event.c
 *
 * PURPOSE:
 *   Implement the test trace event behavior for
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
#include "umicom/observability/performance/trace_event.h"

int main(void) {
    UmiPerformanceTraceEvent left;
    UmiPerformanceTraceEvent right;
    if (umi_performance_trace_event_init(&left, "trace_event", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_trace_event_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_trace_event_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_trace_event_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_trace_event_init(&right, "trace_event", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_trace_event_same_identity(&left, &right)) return 6;
    puts("trace_event: ok");
    return 0;
}
