/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_io_telemetry.c
 *
 * PURPOSE:
 *   Implement the test io telemetry behavior for
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
#include "umicom/observability/performance/io_telemetry.h"

int main(void) {
    UmiPerformanceIoTelemetry left;
    UmiPerformanceIoTelemetry right;
    if (umi_performance_io_telemetry_init(&left, "io_telemetry", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_io_telemetry_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_io_telemetry_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_io_telemetry_ratio(10.0, 2.0) != 5.0) return 4;
    if (umi_performance_io_telemetry_init(&right, "io_telemetry", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_io_telemetry_same_identity(&left, &right)) return 6;
    puts("io_telemetry: ok");
    return 0;
}
