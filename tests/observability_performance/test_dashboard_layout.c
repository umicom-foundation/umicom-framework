#include <stdio.h>
#include "umicom/observability/performance/dashboard_layout.h"

int main(void) {
    UmiPerformanceDashboardLayout left;
    UmiPerformanceDashboardLayout right;
    if (umi_performance_dashboard_layout_init(&left, "dashboard_layout", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_dashboard_layout_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_dashboard_layout_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_dashboard_layout_capacity_available(1U, 2U)) return 4;
    if (umi_performance_dashboard_layout_init(&right, "dashboard_layout", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_dashboard_layout_same_identity(&left, &right)) return 6;
    puts("dashboard_layout: ok");
    return 0;
}
