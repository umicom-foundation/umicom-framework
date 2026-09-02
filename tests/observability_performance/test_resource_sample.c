/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_resource_sample.c
 *
 * PURPOSE:
 *   Implement the test resource sample behavior for
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
#include "umicom/observability/performance/resource_sample.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceResourceSample left;
    UmiPerformanceResourceSample right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_resource_sample_init(&left, "resource_sample", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_resource_sample_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_resource_sample_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_resource_sample_ratio(10.0, 2.0) != 5.0) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_resource_sample_init(&right, "resource_sample", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_resource_sample_same_identity(&left, &right)) return 6;
    puts("resource_sample: ok");
    return 0;
}
