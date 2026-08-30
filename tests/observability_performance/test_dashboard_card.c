/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_dashboard_card.c
 *
 * PURPOSE:
 *   Implement the test dashboard card behavior for
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
#include "umicom/observability/performance/dashboard_card.h"

int main(void) {
    UmiPerformanceDashboardCard left;
    UmiPerformanceDashboardCard right;
    if (umi_performance_dashboard_card_init(&left, "dashboard_card", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_dashboard_card_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_dashboard_card_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_dashboard_card_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_dashboard_card_init(&right, "dashboard_card", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_dashboard_card_same_identity(&left, &right)) return 6;
    puts("dashboard_card: ok");
    return 0;
}
