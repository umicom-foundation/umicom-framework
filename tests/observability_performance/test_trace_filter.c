/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_trace_filter.c
 *
 * PURPOSE:
 *   Implement the test trace filter behavior for
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
#include "umicom/observability/performance/trace_filter.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceTraceFilter left;
    UmiPerformanceTraceFilter right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_trace_filter_init(&left, "trace_filter", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_trace_filter_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_trace_filter_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_trace_filter_transition_allowed(UMI_PERFORMANCE_STATE_IDLE, UMI_PERFORMANCE_STATE_ACTIVE)) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_trace_filter_init(&right, "trace_filter", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_trace_filter_same_identity(&left, &right)) return 6;
    puts("trace_filter: ok");
    return 0;
}
