/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_correlation_context.c
 *
 * PURPOSE:
 *   Implement the test correlation context behavior for
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
#include "umicom/observability/performance/correlation_context.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceCorrelationContext left;
    UmiPerformanceCorrelationContext right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_correlation_context_init(&left, "correlation_context", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_correlation_context_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_correlation_context_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_correlation_context_fingerprint("evidence") == 0U) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_correlation_context_init(&right, "correlation_context", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_correlation_context_same_identity(&left, &right)) return 6;
    puts("correlation_context: ok");
    return 0;
}
