/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_trace_session.c
 *
 * PURPOSE:
 *   Implement the test trace session behavior for
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
#include "umicom/observability/performance/trace_session.h"

int main(void) {
    UmiPerformanceTraceSession left;
    UmiPerformanceTraceSession right;
    if (umi_performance_trace_session_init(&left, "trace_session", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_trace_session_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_trace_session_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_trace_session_duration_ns(10U, 25U) != 15U) return 4;
    if (umi_performance_trace_session_init(&right, "trace_session", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_trace_session_same_identity(&left, &right)) return 6;
    puts("trace_session: ok");
    return 0;
}
