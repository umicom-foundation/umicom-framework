/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_coverage_regression.c
 *
 * PURPOSE:
 *   Implement the test coverage regression behavior for
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
#include "umicom/observability/performance/coverage_regression.h"

int main(void) {
    UmiPerformanceCoverageRegression left;
    UmiPerformanceCoverageRegression right;
    if (umi_performance_coverage_regression_init(&left, "coverage_regression", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_coverage_regression_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_coverage_regression_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_coverage_regression_regression_percent(120.0, 100.0) != 20.0) return 4;
    if (umi_performance_coverage_regression_init(&right, "coverage_regression", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_coverage_regression_same_identity(&left, &right)) return 6;
    puts("coverage_regression: ok");
    return 0;
}
