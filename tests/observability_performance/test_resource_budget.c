#include <stdio.h>
#include "umicom/observability/performance/resource_budget.h"

int main(void) {
    UmiPerformanceResourceBudget left;
    UmiPerformanceResourceBudget right;
    if (umi_performance_resource_budget_init(&left, "resource_budget", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_resource_budget_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_resource_budget_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_resource_budget_within_budget(5.0, 10.0)) return 4;
    if (umi_performance_resource_budget_init(&right, "resource_budget", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_resource_budget_same_identity(&left, &right)) return 6;
    puts("resource_budget: ok");
    return 0;
}
