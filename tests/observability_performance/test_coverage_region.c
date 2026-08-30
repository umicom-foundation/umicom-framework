/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_coverage_region.c
 *
 * PURPOSE:
 *   Implement the test coverage region behavior for
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
#include "umicom/observability/performance/coverage_region.h"

int main(void) {
    UmiPerformanceCoverageRegion left;
    UmiPerformanceCoverageRegion right;
    if (umi_performance_coverage_region_init(&left, "coverage_region", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_coverage_region_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_coverage_region_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_coverage_region_coverage_percent(80U, 100U) != 80.0) return 4;
    if (umi_performance_coverage_region_init(&right, "coverage_region", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_coverage_region_same_identity(&left, &right)) return 6;
    puts("coverage_region: ok");
    return 0;
}
