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

int main(void) {
    UmiPerformanceTraceSpan left;
    UmiPerformanceTraceSpan right;
    if (umi_performance_trace_span_init(&left, "trace_span", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_trace_span_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_trace_span_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_trace_span_duration_ns(10U, 25U) != 15U) return 4;
    if (umi_performance_trace_span_init(&right, "trace_span", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_trace_span_same_identity(&left, &right)) return 6;
    puts("trace_span: ok");
    return 0;
}
