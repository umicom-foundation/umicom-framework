/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_runtime_health.c
 *
 * PURPOSE:
 *   Implement the test runtime health behavior for
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
#include "umicom/observability/performance/runtime_health.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceRuntimeHealth left;
    UmiPerformanceRuntimeHealth right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_runtime_health_init(&left, "runtime_health", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_runtime_health_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_runtime_health_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_performance_runtime_health_healthy(UMI_PERFORMANCE_SEVERITY_WARNING, false)) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_runtime_health_init(&right, "runtime_health", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_runtime_health_same_identity(&left, &right)) return 6;
    puts("runtime_health: ok");
    return 0;
}
