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

int main(void) {
    UmiPerformanceMemoryPressure left;
    UmiPerformanceMemoryPressure right;
    if (umi_performance_memory_pressure_init(&left, "memory_pressure", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_memory_pressure_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_memory_pressure_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_memory_pressure_exceeds(11.0, 10.0)) return 4;
    if (umi_performance_memory_pressure_init(&right, "memory_pressure", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_memory_pressure_same_identity(&left, &right)) return 6;
    puts("memory_pressure: ok");
    return 0;
}
