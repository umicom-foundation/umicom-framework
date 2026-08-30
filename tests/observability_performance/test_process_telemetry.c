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

int main(void) {
    UmiPerformanceProcessTelemetry left;
    UmiPerformanceProcessTelemetry right;
    if (umi_performance_process_telemetry_init(&left, "process_telemetry", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_process_telemetry_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_process_telemetry_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_process_telemetry_ratio(10.0, 2.0) != 5.0) return 4;
    if (umi_performance_process_telemetry_init(&right, "process_telemetry", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_process_telemetry_same_identity(&left, &right)) return 6;
    puts("process_telemetry: ok");
    return 0;
}
