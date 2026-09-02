/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_allocation_summary.c
 *
 * PURPOSE:
 *   Implement the test allocation summary behavior for
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
#include "umicom/observability/performance/allocation_summary.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceAllocationSummary left;
    UmiPerformanceAllocationSummary right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_allocation_summary_init(&left, "allocation_summary", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_allocation_summary_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_allocation_summary_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_allocation_summary_mean(10.0, 2U) != 5.0) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_allocation_summary_init(&right, "allocation_summary", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_allocation_summary_same_identity(&left, &right)) return 6;
    puts("allocation_summary: ok");
    return 0;
}
