/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_gauge.c
 *
 * PURPOSE:
 *   Implement the test gauge behavior for
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
#include "umicom/observability/performance/gauge.h"

int main(void) {
    UmiPerformanceGauge left;
    UmiPerformanceGauge right;
    if (umi_performance_gauge_init(&left, "gauge", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_gauge_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_gauge_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_gauge_exceeds(11.0, 10.0)) return 4;
    if (umi_performance_gauge_init(&right, "gauge", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_gauge_same_identity(&left, &right)) return 6;
    puts("gauge: ok");
    return 0;
}
