/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_crash_cluster.c
 *
 * PURPOSE:
 *   Implement the test crash cluster behavior for
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
#include "umicom/observability/performance/crash_cluster.h"

int main(void) {
    UmiPerformanceCrashCluster left;
    UmiPerformanceCrashCluster right;
    if (umi_performance_crash_cluster_init(&left, "crash_cluster", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_crash_cluster_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_crash_cluster_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_crash_cluster_capacity_available(1U, 2U)) return 4;
    if (umi_performance_crash_cluster_init(&right, "crash_cluster", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_crash_cluster_same_identity(&left, &right)) return 6;
    puts("crash_cluster: ok");
    return 0;
}
