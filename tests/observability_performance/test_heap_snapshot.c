/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_heap_snapshot.c
 *
 * PURPOSE:
 *   Implement the test heap snapshot behavior for
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
#include "umicom/observability/performance/heap_snapshot.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceHeapSnapshot left;
    UmiPerformanceHeapSnapshot right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_heap_snapshot_init(&left, "heap_snapshot", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_heap_snapshot_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_heap_snapshot_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_heap_snapshot_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_heap_snapshot_init(&right, "heap_snapshot", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_heap_snapshot_same_identity(&left, &right)) return 6;
    puts("heap_snapshot: ok");
    return 0;
}
