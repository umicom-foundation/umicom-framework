/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_trace_span.c
 *
 * PURPOSE:
 *   Implement the test trace span behavior for
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
#include "umicom/observability/performance/trace_span.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceTraceSpan left;
    UmiPerformanceTraceSpan right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_trace_span_init(&left, "trace_span", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_trace_span_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_trace_span_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_trace_span_duration_ns(10U, 25U) != 15U) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_trace_span_init(&right, "trace_span", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_trace_span_same_identity(&left, &right)) return 6;
    puts("trace_span: ok");
    return 0;
}
