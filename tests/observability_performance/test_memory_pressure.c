/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_memory_pressure.c
 *
 * PURPOSE:
 *   Implement the test memory pressure behavior for
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
#include "umicom/observability/performance/memory_pressure.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceMemoryPressure left;
    UmiPerformanceMemoryPressure right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_memory_pressure_init(&left, "memory_pressure", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_memory_pressure_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_memory_pressure_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_performance_memory_pressure_exceeds(11.0, 10.0)) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_memory_pressure_init(&right, "memory_pressure", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_memory_pressure_same_identity(&left, &right)) return 6;
    puts("memory_pressure: ok");
    return 0;
}
