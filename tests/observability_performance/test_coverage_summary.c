/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_coverage_summary.c
 *
 * PURPOSE:
 *   Implement the test coverage summary behavior for
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
#include "umicom/observability/performance/coverage_summary.h"

int main(void) {
    UmiPerformanceCoverageSummary left;
    UmiPerformanceCoverageSummary right;
    if (umi_performance_coverage_summary_init(&left, "coverage_summary", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_coverage_summary_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_coverage_summary_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_coverage_summary_coverage_percent(80U, 100U) != 80.0) return 4;
    if (umi_performance_coverage_summary_init(&right, "coverage_summary", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_coverage_summary_same_identity(&left, &right)) return 6;
    puts("coverage_summary: ok");
    return 0;
}
