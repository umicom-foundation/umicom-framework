/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_allocation_site.c
 *
 * PURPOSE:
 *   Implement the test allocation site behavior for
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
#include "umicom/observability/performance/allocation_site.h"

int main(void) {
    UmiPerformanceAllocationSite left;
    UmiPerformanceAllocationSite right;
    if (umi_performance_allocation_site_init(&left, "allocation_site", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_allocation_site_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_allocation_site_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_allocation_site_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_allocation_site_init(&right, "allocation_site", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_allocation_site_same_identity(&left, &right)) return 6;
    puts("allocation_site: ok");
    return 0;
}
