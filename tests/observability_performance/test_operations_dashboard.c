/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_operations_dashboard.c
 *
 * PURPOSE:
 *   Implement the test operations dashboard behavior for
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
#include "umicom/observability/performance/operations_dashboard.h"

int main(void) {
    UmiPerformanceOperationsDashboard left;
    UmiPerformanceOperationsDashboard right;
    if (umi_performance_operations_dashboard_init(&left, "operations_dashboard", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_operations_dashboard_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_operations_dashboard_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_operations_dashboard_healthy(UMI_PERFORMANCE_SEVERITY_WARNING, false)) return 4;
    if (umi_performance_operations_dashboard_init(&right, "operations_dashboard", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_operations_dashboard_same_identity(&left, &right)) return 6;
    puts("operations_dashboard: ok");
    return 0;
}
