/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_process_telemetry.c
 *
 * PURPOSE:
 *   Implement the test process telemetry behavior for
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
#include "umicom/observability/performance/process_telemetry.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceProcessTelemetry left;
    UmiPerformanceProcessTelemetry right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_process_telemetry_init(&left, "process_telemetry", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_process_telemetry_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_process_telemetry_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_process_telemetry_ratio(10.0, 2.0) != 5.0) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_process_telemetry_init(&right, "process_telemetry", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_process_telemetry_same_identity(&left, &right)) return 6;
    puts("process_telemetry: ok");
    return 0;
}
