/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_dashboard_layout.c
 *
 * PURPOSE:
 *   Implement the test dashboard layout behavior for
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
#include "umicom/observability/performance/dashboard_layout.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPerformanceDashboardLayout left;
    UmiPerformanceDashboardLayout right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_dashboard_layout_init(&left, "dashboard_layout", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_dashboard_layout_validate(&left) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_dashboard_layout_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_performance_dashboard_layout_capacity_available(1U, 2U)) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_dashboard_layout_init(&right, "dashboard_layout", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_dashboard_layout_same_identity(&left, &right)) return 6;
    puts("dashboard_layout: ok");
    return 0;
}
