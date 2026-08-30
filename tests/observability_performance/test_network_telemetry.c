/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_network_telemetry.c
 *
 * PURPOSE:
 *   Implement the test network telemetry behavior for
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
#include "umicom/observability/performance/network_telemetry.h"

int main(void) {
    UmiPerformanceNetworkTelemetry left;
    UmiPerformanceNetworkTelemetry right;
    if (umi_performance_network_telemetry_init(&left, "network_telemetry", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_network_telemetry_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_network_telemetry_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_network_telemetry_ratio(10.0, 2.0) != 5.0) return 4;
    if (umi_performance_network_telemetry_init(&right, "network_telemetry", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_network_telemetry_same_identity(&left, &right)) return 6;
    puts("network_telemetry: ok");
    return 0;
}
