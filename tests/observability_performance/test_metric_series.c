/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_metric_series.c
 *
 * PURPOSE:
 *   Implement the test metric series behavior for
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
#include "umicom/observability/performance/metric_series.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceMetricSeries left;
    UmiPerformanceMetricSeries right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_metric_series_init(&left, "metric_series", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_metric_series_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_metric_series_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_performance_metric_series_capacity_available(1U, 2U)) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_metric_series_init(&right, "metric_series", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_metric_series_same_identity(&left, &right)) return 6;
    puts("metric_series: ok");
    return 0;
}
