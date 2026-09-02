/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_coverage_file.c
 *
 * PURPOSE:
 *   Implement the test coverage file behavior for
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
#include "umicom/observability/performance/coverage_file.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceCoverageFile left;
    UmiPerformanceCoverageFile right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_coverage_file_init(&left, "coverage_file", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_coverage_file_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_coverage_file_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_coverage_file_coverage_percent(80U, 100U) != 80.0) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_coverage_file_init(&right, "coverage_file", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_coverage_file_same_identity(&left, &right)) return 6;
    puts("coverage_file: ok");
    return 0;
}
