#include <stdio.h>
#include "umicom/observability/performance/allocation_summary.h"

int main(void) {
    UmiPerformanceAllocationSummary left;
    UmiPerformanceAllocationSummary right;
    if (umi_performance_allocation_summary_init(&left, "allocation_summary", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_allocation_summary_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_allocation_summary_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_allocation_summary_mean(10.0, 2U) != 5.0) return 4;
    if (umi_performance_allocation_summary_init(&right, "allocation_summary", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_allocation_summary_same_identity(&left, &right)) return 6;
    puts("allocation_summary: ok");
    return 0;
}
