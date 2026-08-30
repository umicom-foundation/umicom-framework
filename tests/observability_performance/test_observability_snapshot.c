/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_observability_snapshot.c
 *
 * PURPOSE:
 *   Implement the test observability snapshot behavior for
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
#include "umicom/observability/performance/observability_snapshot.h"

int main(void) {
    UmiPerformanceObservabilitySnapshot left;
    UmiPerformanceObservabilitySnapshot right;
    if (umi_performance_observability_snapshot_init(&left, "observability_snapshot", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_observability_snapshot_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_observability_snapshot_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_observability_snapshot_fingerprint("evidence") == 0U) return 4;
    if (umi_performance_observability_snapshot_init(&right, "observability_snapshot", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_observability_snapshot_same_identity(&left, &right)) return 6;
    puts("observability_snapshot: ok");
    return 0;
}
